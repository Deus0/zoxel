// Gpu meshing of vox model lods (doc/plans/chunks_refactor.md phase 4): a GpuMesh VoxLodMesh (zox_vox_gpu_meshing)
//  is meshed by the compute shader (fun/gpu_mesh.c) straight into its own buffers, the cpu's build skips it
//  - its model's voxels at its lod filled into a grid, its faces counted on the cpu (gpu_mesh_grid_faces, the
//      shader's rule) and its buffers grown to fit (MeshFaceRoom) before the dispatch: it never overflows, and its
//      count is known without reading back (MeshRenderCount, its indices drawn)
//  - its kind's voxels only (a TransparentMesh's see through ones), coloured by its palette with ambient occlusion
//  - vertices in the model's space as the cpu's (bounds centred), vec4s (vox_instance_render reads them strided),
//      colours rgba8
//  - its MeshVertices only its drawn voxels' two far corners: its size for culling (instance_mesh_radius) and framing
//  - one mesher (the program and its grid, command and count buffers) for every model, made on the first mesh,
//      disposed with the gpu (gpu_dispose_vox_gpu), its meshes meshed again after (gpu_restore_vox_gpu)
//  - a main thread budget a frame shared by lods and skeleton meshes (a realm's models spawned at once), the rest next
//      frame; at least one a frame
GpuMesher zox_vox_gpu_mesher;
double zox_vox_gpu_budget_ms = 2.0;
uint zox_vox_gpu_budget_frame = UINT32_MAX;
double zox_vox_gpu_budget_used = 0;
byte zox_vox_gpu_rush = 0;      // after a gpu restore: every mesh in the next frame, as a cpu mesh's upload is

// this frame's budget spent: leave the rest for the next
static inline byte vox_gpu_budget_spent() {
    if (zox_vox_gpu_budget_frame != ecs_run_count) {
        zox_vox_gpu_budget_frame = ecs_run_count;
        zox_vox_gpu_budget_used = 0;
        zox_vox_gpu_rush = zox_vox_gpu_rush == 1 ? 2 : 0;   // the frame after it was asked for
    }
    return zox_vox_gpu_budget_used >= zox_vox_gpu_budget_ms && !zox_disable_process_skips && !zox_vox_gpu_rush;
}

// a model's grids at depth (gpu_mesh_grid's layout, a cell of air round it): voxels its node values, sides each cell's
//  sides that hide a kind's faces, bit d its side facing d all hiding at full detail (get_node_sides_all_hide), as the
//  cpu's colored walk hides a face at a lod
// a node's sides that hide a kind's faces, bit d its side facing d
static inline byte vox_gpu_sides(const byte* materials, byte kind, const VoxelNode* node) {
    byte hides = 0;
    for (byte d = 0; d < 6; d++) {
        hides |= get_node_sides_all_hide(materials, kind, node, d >> 1, d & 1, 8) << d;
    }
    return hides;
}

static void vox_gpu_fill(const VoxelNode* node, const byte* materials, byte kind, byte depth, byte target, int3 p,
    byte* voxels, byte* sides, int length)
{
    if (depth < target && has_children_VoxelNode(node)) {
        const VoxelNode* kids = get_children_VoxelNode(node);
        for (byte i = 0; i < octree_length; i++) {
            const byte3 o = octree_positions[i];
            vox_gpu_fill(&kids[i], materials, kind, depth + 1, target, (int3) { p.x * 2 + o.x, p.y * 2 + o.y, p.z * 2 + o.z },
                voxels, sides, length);
        }
        return;
    }
    const byte hides = vox_gpu_sides(materials, kind, node);
    const int span = 1 << (target - depth);
    for (int x = p.x * span; x < (p.x + 1) * span; x++)
    for (int y = p.y * span; y < (p.y + 1) * span; y++)
    for (int z = p.z * span; z < (p.z + 1) * span; z++) {
        const int i = ((x + 1) * length + y + 1) * length + z + 1;
        voxels[i] = node->value;
        sides[i] = hides;
    }
}

// the faces the shader writes for a model's grids (its rule with sides), a mesh's room before it's dispatched;
//  extent: the drawn cells' lower and upper corners
static uint vox_gpu_faces(const GpuMesher* mesher, const byte* voxels, const byte* sides, byte depth, int3* extent) {
    const int size = octree_size(depth);
    const int length = size + 2;
    // its own directions (move_position called here too made gcc compile the cpu's colored walk 65% slower, release)
    const int3 directions[6] = { { -1, 0, 0 }, { 1, 0, 0 }, { 0, -1, 0 }, { 0, 1, 0 }, { 0, 0, -1 }, { 0, 0, 1 } };
    uint faces = 0;
    extent[0] = int3_single(size);
    extent[1] = int3_zero;
    for (int x = 0; x < size; x++)
    for (int y = 0; y < size; y++)
    for (int z = 0; z < size; z++) {
        const byte voxel = voxels[((x + 1) * length + y + 1) * length + z + 1];
        if (!voxel || !((mesher->drawn[(voxel - 1) >> 5] >> ((voxel - 1) & 31)) & 1)) {
            continue;
        }
        extent[0] = (int3) { int_min(extent[0].x, x), int_min(extent[0].y, y), int_min(extent[0].z, z) };
        extent[1] = (int3) { int_max(extent[1].x, x + 1), int_max(extent[1].y, y + 1), int_max(extent[1].z, z + 1) };
        for (byte d = 0; d < 6; d++) {
            const int3 n = { x + directions[d].x, y + directions[d].y, z + directions[d].z };
            faces += !((sides[((n.x + 1) * length + n.y + 1) * length + n.z + 1] >> (d ^ 1)) & 1);
        }
    }
    return faces;
}

// a neighbour chunk's side against this one into the grid's border: each border cell its sides that hide (faces into it
//  hide as the cpu's, is_adjacent_all_solid), its voxel left air (ambient occlusion stops at the chunk, vox_lookup_solid)
static void vox_gpu_fill_neighbor(const VoxelNode* neighbor, const byte* materials, byte kind, byte depth, byte d,
    byte* sides, int length)
{
    const int size = octree_size(depth);
    const int axis = d >> 1;
    const int edge = d & 1 ? size : -1;         // the border cells' coordinate on its axis
    const int inside = d & 1 ? 0 : size - 1;    // the same cells in the neighbour
    for (int a = 0; a < size; a++)
    for (int b = 0; b < size; b++) {
        int3 cell = axis == 0 ? (int3) { edge, a, b } : axis == 1 ? (int3) { a, edge, b } : (int3) { a, b, edge };
        int3 there = cell;
        if (axis == 0) there.x = inside; else if (axis == 1) there.y = inside; else there.z = inside;
        const VoxelNode* node = get_VoxelNode(neighbor, depth, int3_to_byte3(there));
        sides[((cell.x + 1) * length + cell.y + 1) * length + cell.z + 1] = node ? vox_gpu_sides(materials, kind, node) : 0;
    }
}

// a model's lod into a mesher (its buffers sized for faces before, place set), faces returned; 0: nothing dispatched
//  - materials: vox_material_table's, kind the mesh's
//  - neighbors: a chunk's 6 neighbour chunks' trees at depth (a colored chunk3 parent's), NULL a model alone
//  - bones: its cells' bones (gpu_mesh_grid's layout, 255 air and round it) to skin it (mesher's skinned, parents set),
//      NULL unskinned
static uint vox_gpu_mesh_grid(GpuMesher* mesher, const VoxelNode* root, byte depth, const byte* materials, byte kind,
    const ColorRGBs* palette, uint (*room)(GpuMesher* mesher, uint faces, void* data), void* data, int3* extent,
    const byte* bones, const VoxelNode** neighbors)
{
    const int length = octree_size(depth) + 2;
    const size_t cells = gpu_mesh_grid_bytes(depth);
    byte* grid = calloc(cells, 1);
    byte* sides = calloc(bones ? cells * 2 : cells, 1);     // its bones after its sides
    if (bones) {
        memcpy(sides + cells, bones, cells);
    }
    mesher->skinned = bones != NULL;
    vox_gpu_fill(root, materials, kind, 0, depth, int3_zero, grid, sides, length);
    // the nearby chunks there (OCTREE_INDEX's order), faces into the rest show
    uint present = 1u << 13;
    for (byte d = 0; neighbors && d < 6; d++) {
        if (neighbors[d]) {
            vox_gpu_fill_neighbor(neighbors[d], materials, kind, depth, d, sides, length);
            const int s = d & 1 ? 1 : -1;
            present |= 1u << (13 + (d >> 1 == 0 ? s : d >> 1 == 1 ? s * 3 : s * 9));
        }
    }
    gpu_mesher_set_material(mesher, materials, kind);
    gpu_mesher_set_colors(mesher, palette->value, palette->length, materials);
    mesher->sided = 1;
    mesher->edge_faces = 1;
    const uint faces = vox_gpu_faces(mesher, grid, sides, depth, extent);
    if (faces && room(mesher, faces, data)) {
        gpu_mesh_grid(mesher, grid, sides, depth, present, 0, NULL);
    }
    free(grid);
    free(sides);
    return faces;
}

// the shared mesher, 0 without compute (the mesh goes back to the cpu)
static inline GpuMesher* vox_gpu_mesher() {
    if (!zox_vox_gpu_mesher.program) {
        if (!gpu_mesher_create(&zox_vox_gpu_mesher, 6)) {
            return NULL;
        }
        // greedy's scratch made with it (a gpu restore counts it back at once), for 32³ models, grown in place
        zox_vox_gpu_mesher.scratch_bytes = gpu_mesh_greedy_bytes(5);
        zox_vox_gpu_mesher.scratch = zox_gpu_storage_buffer(zox_vox_gpu_mesher.scratch_bytes, NULL);
    }
    return &zox_vox_gpu_mesher;
}

typedef struct {
    guint2 mesh;
    guint colors;
    guint weights;  // a skinned mesh's bones, a uint a vertex (BoneIndexGPULink), 0 unskinned
    uint* room;
} vox_gpu_room_data;

// a lod mesh's buffers grown to fit faces (at least a voxel's: vulkan can't bind empty buffers), 1: dispatch
static uint vox_gpu_room(GpuMesher* mesher, uint faces, void* data) {
    vox_gpu_room_data* d = data;
    if (faces > *d->room) {
        *d->room = faces > 6 ? faces : 6;
        zox_gpu_storage_upload(d->mesh.x, (size_t) *d->room * 6 * sizeof(uint), NULL);
        zox_gpu_storage_upload(d->mesh.y, (size_t) *d->room * 4 * sizeof(float4), NULL);
        zox_gpu_storage_upload(d->colors, (size_t) *d->room * 4 * sizeof(uint), NULL);
        if (d->weights) {
            zox_gpu_storage_upload(d->weights, (size_t) *d->room * 4 * sizeof(uint), NULL);
        }
    }
    mesher->max_faces = *d->room;
    return 1;
}

// a skeleton mesh's skinning (bones/sys/skeleton_gpu.c fills it): grid makes its vox's bones grid at depth (the
//  vox locked)
typedef struct {
    guint weights;
    uint bones_count;
    uint parents[64];
    byte* (*grid)(ecs* world, entity vox, byte depth, const void* data);
    const void* data;
} vox_gpu_skin;

// meshes a tree at a depth into a mesh's buffers (mesh: indices x, vertices y; colors), its faces returned; its trees
//  locked by the caller
//  - size, scale: its ChunkSize and BlockScale (its space centred, as the cpu's); bounds_out its drawn voxels' corners
//  - bones: a skeleton mesh's bones grid (skin's weights, parents), NULL none
//  - command, fence: its own count buffers (MeshCommandGPULink, made here) and MeshFence: with greedy meshing on its
//      faces merge and their count comes back later (vox_gpu_count_system); NULL: every face, its count returned
static uint vox_gpu_mesh_tree(const VoxelNode* root, const VoxelNode** neighbors, const ColorRGBs* palette,
    const byte* materials, byte node_depth, byte depth, byte kind, int3 size, float scale, guint2 mesh, guint colors,
    uint* room, float3* bounds_out, const vox_gpu_skin* skin, const byte* bones, guint2* command, ulong* fence)
{
    GpuMesher* shared = vox_gpu_mesher();
    if (!shared || !palette->length) {
        return 0;
    }
    // the shared mesher, writing into this mesh's buffers
    GpuMesher mesher = *shared;
    mesher.indices = mesh.x;
    mesher.vertices = mesh.y;
    mesher.vertex_colors = colors;
    mesher.fence = 0;
    const float3 bounds = calculate_vox_bounds(size, scale);
    const float voxel = scale * octree_size(node_depth) / octree_size(depth);
    mesher.place = (float4) { -bounds.x, -bounds.y, -bounds.z, voxel };
    if (skin) {
        mesher.vertex_lights = skin->weights;
        mesher.bones_count = skin->bones_count;
        memcpy(mesher.parents, skin->parents, sizeof(mesher.parents));
    }
    // merged on the gpu up to 64³ (its scratch, a key a face: 12MB there, 100MB at 128³)
    const byte greedy = zox_greedy_mesh && command && fence && depth <= zox_greedy_depth_max;
    // its count buffers made with its first mesh, merged or not (terrain's gpu restore makes them again by its room)
    if (command && !command->x) {
        *command = (guint2) { zox_gpu_storage_buffer(sizeof(zox_gpu_mesh_command_reset), zox_gpu_mesh_command_reset),
            zox_gpu_storage_buffer_read(sizeof(uint), &zox_gpu_mesh_count_reset) };
    }
    if (greedy) {
        mesher.command = command->x;
        mesher.count = command->y;
        mesher.greedy = 1;
    }
    vox_gpu_room_data data = { mesh, colors, skin ? skin->weights : 0, room };
    int3 extent[2];
    // its unmerged faces: room for the most it writes merged
    const uint faces = vox_gpu_mesh_grid(&mesher, root, depth, materials, kind, palette, vox_gpu_room, &data, extent,
        bones, neighbors);
    shared->scratch_bytes = mesher.scratch_bytes;   // grown in place
    if (faces) {
        for (byte b = 0; b < 2; b++) {
            bounds_out[b] = float3_add(float3_scale(float3_from_int3(extent[b]), voxel), float3_scale(bounds, -1));
        }
        if (greedy) {
            zox_gpu_fence_dispose(*fence);
            *fence = mesher.fence;
        } else {
            zox_gpu_fence_dispose(mesher.fence);
        }
    }
    return faces;
}

// meshes one lod of a model into its buffers (mesh: indices x, vertices y; colors), its faces returned
//  - bounds: its drawn voxels' lower and upper corners in its space
//  - skin: a skeleton mesh's (bones/sys/skeleton_gpu.c), its weights buffer, its bones grid made at depth, NULL none
uint vox_gpu_mesh(ecs* world, entity vox, byte depth, byte kind, guint2 mesh, guint colors, uint* room, float3* bounds_out,
    const vox_gpu_skin* skin, guint2* command, ulong* fence)
{
    if (!zox_valid(vox) || !zox_has(vox, VoxelNode) || !zox_has(vox, ColorRGBs) || !zox_has(vox, ChunkSize)) {
        return 0;
    }
    const ColorRGBs* palette = zox_get(vox, ColorRGBs);
    const byte node_depth = zox_getv(vox, NodeDepth);
    if (depth > node_depth) {
        depth = node_depth;
    }
    byte materials[256];
    const byte other = vox_material_table(zox_has(vox, MaterialIDs) ? zox_get(vox, MaterialIDs) : NULL, materials);
    if (!other && kind != zox_block_material_solid) {
        return 0;   // no see through voxels
    }
    spinlock* lock = zox_octree_lock(world, vox);
    zox_octree_lock_begin(lock);
    byte* bones = skin ? skin->grid(world, vox, depth, skin->data) : NULL;
    const uint faces = vox_gpu_mesh_tree(zox_get(vox, VoxelNode), NULL, palette, materials, node_depth, depth, kind,
        zox_getv(vox, ChunkSize), zox_getv(vox, BlockScale), mesh, colors, room, bounds_out, skin, bones, command, fence);
    zox_octree_lock_end(lock);
    free(bones);
    return faces;
}

// a gpu mesh's count buffers and fence (MeshCommandGPULink, MeshFence) for greedy meshing, NULL without them
static inline guint2* vox_gpu_command_of(ecs* world, entity e) {
    return zox_has(e, MeshCommandGPULink) ? &zox_get_mut(e, MeshCommandGPULink)->value : NULL;
}

static inline ulong* vox_gpu_fence_of(ecs* world, entity e) {
    return zox_has(e, MeshFence) ? &zox_get_mut(e, MeshFence)->value : NULL;
}

// is a mesh's count coming back (greedy: its faces merged, vox_gpu_count_system reads it)
static inline byte vox_gpu_pending(ecs* world, entity e) {
    return zox_has(e, MeshFence) && zox_getv(e, MeshFence);
}

// a mesh just meshed: its corners (MeshVertices, culling), and its count now unless it comes back later (greedy)
static inline void vox_gpu_meshed(ecs* world, entity e, uint faces, const float3* bounds, MeshRenderCount* count,
    MeshVertices* vertices)
{
    resize_MeshVertices(vertices, faces ? 2 : 0);
    if (faces) {
        vertices->value[0] = bounds[0];
        vertices->value[1] = bounds[1];
    }
    if (!faces || !vox_gpu_pending(world, e)) {
        count->value = faces * 6;
        mesh_clones_sync_gpu(world, e);
    }
}

void vox_lod_gpu_mesh_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_in(MeshGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_out(MeshFaceRoom);
    zox_sys_out(MeshRenderCount);
    zox_sys_out(MeshVertices);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(ColorsGPULink, colors);
        zox_sys_o(MeshFaceRoom, room);
        zox_sys_o(MeshRenderCount, count);
        zox_sys_o(MeshVertices, vertices);
        if (!mesh->value.x || !mesh->value.y || !colors->value) {
            continue;   // its buffers come with its initialize
        }
        const entity vox = zox_get_parent(world, e);
        if (zox_has(vox, ColorRGBs) && !zox_get(vox, ColorRGBs)->length) {
            continue;   // voxels not ready, built again on VoxelNodeDirty
        }
        if (vox_gpu_budget_spent()) {
            continue;
        }
        const double begin = get_time_ms();
        const byte kind = zox_has(e, TransparentMesh) ? zox_block_material_liquid : zox_block_material_solid;
        float3 bounds[2];
        const uint faces = vox_gpu_mesh(world, vox, depth->value, kind, mesh->value, colors->value, &room->value, bounds,
            NULL, vox_gpu_command_of(world, e), vox_gpu_fence_of(world, e));
        zox_vox_gpu_budget_used += get_time_ms() - begin;
        vox_gpu_meshed(world, e, faces, bounds, count, vertices);
        zox_remove(e, BuildMesh);
    }
    zox_sys_on_end();
} zoxd_system(vox_lod_gpu_mesh_system);

byte zox_vox_gpu_mesher_disposed = 0;    // made again with the gpu

// the gpu's gone (android's context lost): the mesher goes with it
void gpu_dispose_vox_gpu(ecs* world) {
    (void) world;
    if (zox_vox_gpu_mesher.program) {
        gpu_mesher_dispose(&zox_vox_gpu_mesher);
        zox_vox_gpu_mesher_disposed = 1;
    }
}

// a new gpu: the mesher made again (as it was), every gpu meshed lod meshed again (their buffers made anew by
//  rendering's restore, its cpu uploads taken off: their arrays are empty)
void gpu_restore_vox_gpu(ecs* world) {
    if (zox_vox_gpu_mesher_disposed) {
        zox_vox_gpu_mesher_disposed = 0;
        vox_gpu_mesher();
    }
    zox_vox_gpu_rush = 1;
    zox_each_begin(MeshFaceRoom);
        zox_each_o(MeshFaceRoom, room);
        if (zox_has(e, VoxLodMesh) || zox_has(e, SkeletonMesh) || zox_has(e, ColorChunk)) {
            // one with count buffers: terrain's restore makes them again by its room, and zeroes it (gpu_restore.c)
            if (!zox_has(e, MeshCommandGPULink)) {
                room->value = 0;
            }
            zox_setv(e, MeshRenderCount, 0);
            zox_remove(e, MeshDirty);
            zox_remove(e, SkeletonMeshDirty);
            zox_add(e, BuildMesh);
        }
    zox_each_end();
}

// a colored chunk's see through child (set_chunk3_parent_colored_materials), 0 if it has none
static inline entity vox_gpu_shell_of(ecs* world, entity chunk) {
    iter it = zox_children(world, chunk);
    while (zox_children_next(it)) {
        for (int j = 0; j < it.count; j++) {
            if (zox_has(it.entities[j], TransparentMesh)) {
                const entity shell = it.entities[j];
                zox_children_end(it);
                return shell;
            }
        }
    }
    return 0;
}

// a colored chunk3 parent's chunk (a model bigger than a chunk, spawn_chunk3_parent_colored_chunk) on the gpu: its solid
//  voxels into its own mesh, its see through ones into its see through child, faces into its neighbours hidden as the
//  cpu's (chunk_colors_build_system); its parent's palette and material ids
void chunk_colors_gpu_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNode);
    zox_sys_in(NodeDepth);
    zox_sys_in(RenderDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_in(ColorRGBs);
    zox_sys_in(ChunkSize);
    zox_sys_in(BlockScale);
    zox_sys_in(MeshGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_out(MeshFaceRoom);
    zox_sys_out(MeshRenderCount);
    zox_sys_out(MeshVertices);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNode, voxels);
        zox_sys_i(NodeDepth, ndepth);
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_i(ColorRGBs, vcolors);
        zox_sys_i(ChunkSize, csize);
        zox_sys_i(BlockScale, scale);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(ColorsGPULink, colors);
        zox_sys_o(MeshFaceRoom, room);
        zox_sys_o(MeshRenderCount, count);
        zox_sys_o(MeshVertices, vertices);
        if (!mesh->value.x || !mesh->value.y || !colors->value || vox_gpu_budget_spent()) {
            continue;
        }
        const entity owner = vcolors->length ? e : zox_get_parent(world, e);
        const ColorRGBs* palette = vcolors->length ? vcolors :
            (zox_valid(owner) && zox_has(owner, ColorRGBs) ? zox_get(owner, ColorRGBs) : vcolors);
        if (!palette->length) {
            continue;
        }
        const double begin = get_time_ms();
        const VoxelNode* noctrees[6];
        for (byte j = 0; j < 6; j++) {
            noctrees[j] = neighbors->value[j] ? zox_get(neighbors->value[j], VoxelNode) : NULL;
        }
        byte materials[256];
        const byte other = vox_material_table(zox_has(owner, MaterialIDs) ? zox_get(owner, MaterialIDs) : NULL, materials);
        const byte depth = rdepth->value < ndepth->value ? rdepth->value : ndepth->value;
        spinlock* neighbor_locks[6];
        zox_octree_neighbor_locks(world, neighbors, neighbor_locks);
        spinlock* locked[7];
        const byte locked_count = zox_lock_voxel_trees(zox_octree_lock(world, e), neighbor_locks, locked);
        float3 bounds[2];
        uint faces = vox_gpu_mesh_tree(voxels, noctrees, palette, materials, ndepth->value, depth,
            zox_block_material_solid, csize->value, scale->value, mesh->value, colors->value, &room->value, bounds,
            NULL, NULL, vox_gpu_command_of(world, e), vox_gpu_fence_of(world, e));
        vox_gpu_meshed(world, e, faces, bounds, count, vertices);
        const entity shell = other ? vox_gpu_shell_of(world, e) : 0;
        if (zox_valid(shell) && zox_has(shell, GpuMesh)) {
            const guint2 shell_mesh = zox_getv(shell, MeshGPULink);
            const guint shell_colors = zox_getv(shell, ColorsGPULink);
            if (shell_mesh.x && shell_mesh.y && shell_colors) {
                faces = vox_gpu_mesh_tree(voxels, noctrees, palette, materials, ndepth->value, depth,
                    zox_block_material_liquid, csize->value, scale->value, shell_mesh, shell_colors,
                    &zox_get_mut(shell, MeshFaceRoom)->value, bounds, NULL, NULL, vox_gpu_command_of(world, shell),
                    vox_gpu_fence_of(world, shell));
                vox_gpu_meshed(world, shell, faces, bounds, zox_get_mut(shell, MeshRenderCount),
                    zox_get_mut(shell, MeshVertices));
            }
        }
        zox_unlock_voxel_trees(locked, locked_count);
        zox_remove(e, BuildMesh);
        zox_vox_gpu_budget_used += get_time_ms() - begin;
    }
    zox_sys_on_end();
} zoxd_system(chunk_colors_gpu_system);

// a greedy gpu mesh's merged count, read once the gpu's done (it never waits): then drawn (a skeleton mesh built, its
//  swap can go), its clones copy it
void vox_gpu_count_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshCommandGPULink);
    zox_sys_out(MeshFence);
    zox_sys_out(MeshRenderCount);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshCommandGPULink, command);
        zox_sys_o(MeshFence, fence);
        zox_sys_o(MeshRenderCount, count);
        if (!fence->value || !command->value.y || !zox_gpu_fence_done(fence->value)) {
            continue;
        }
        zox_gpu_fence_dispose(fence->value);
        fence->value = 0;
        uint faces = 0;
        zox_gpu_storage_read(command->value.y, 0, sizeof(uint), &faces);
        const uint room = zox_has(e, MeshFaceRoom) ? zox_getv(e, MeshFaceRoom) : 0;
        count->value = (faces < room ? faces : room) * 6;
        if (zox_has(e, SkeletonMesh)) {
            zox_add(e, MeshBuilt);
        }
        mesh_clones_sync_gpu(world, e);
    }
    zox_sys_on_end();
} zoxd_system(vox_gpu_count_system);
