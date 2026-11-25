// A textured chunk mesh from a voxel grid, no SidesOctree (doc/plans/chunks_refactor.md, phase 2): the game's cpu
//  meshing (zox_chunks3_grid_meshing, --sides_meshing for the old way), measured against sides (tst perf_meshing)
//  - the grid: the chunk's voxels at the mesh's depth as bytes, a voxel of border from its 6 neighbours, each at the
//    depth it draws (as sides read them: chunk_side_depth)
//  - which faces show is the gpu mesher's rule (fun/gpu_mesh.c face_shows, gpu_mesh_grid_faces): a voxel of the mesh's
//    material, beside one that doesn't hide it (block_material_hides); into a missing chunk only its top
//  - faces are written as build_textured.c writes them: the same corners, uvs and lowered tops (flowing water)
typedef struct {
    byte* voxels;       // the caller's buffer, voxel_grid_cells(depth) big: a system keeps one for its run, not a mesh
    int length;         // the chunk's voxels a side at the depth, + 2
    byte present[6];    // which neighbours there are, by direction
} voxel_grid;

// the bytes a grid at depth needs
static inline size_t voxel_grid_cells(byte depth) {
    const size_t length = octree_size(depth) + 2;
    return length * length * length;
}

static inline int voxel_grid_index(int length, int x, int y, int z) {
    return ((x + 1) * length + y + 1) * length + z + 1;
}

// fills a grid at depth: the chunk, then each neighbour's layer touching it, at the depth it draws
//  - drawn coarser: its nodes fill the cells (octree_fill_grid_stop)
//  - drawn finer: a cell is air where any of its voxels along the border doesn't hide the mesh's (hides), as sides'
//    get_node_sides_all_hide; unsettled (chunk_side_open, 255): air, every face into it shows
static inline void voxel_grid_fill(voxel_grid* grid, byte* buffer, const VoxelNode* voxels, const VoxelNode** neighbors,
    const byte* neighbor_depths, byte depth, const byte* hides)
{
    const int size = octree_size(depth);
    grid->length = size + 2;
    grid->voxels = buffer;
    memset(grid->voxels, 0, voxel_grid_cells(depth));
    octree_fill_grid(voxels, sizeof(VoxelNode), octree_type_offset_VoxelNode, 0, depth, int3_zero,
        (int3) { 1, 1, 1 }, grid->voxels, grid->length, 0);
    for (byte d = 0; d < 6; d++) {
        grid->present[d] = neighbors[d] != NULL;
        if (!neighbors[d]) {
            continue;
        }
        const int3 offset = move_position(int3_zero, d);
        const int3 origin = { offset.x * size + 1, offset.y * size + 1, offset.z * size + 1 };
        const byte drawn = neighbor_depths[d];
        const byte open = drawn == 255;
        const byte stop = !open && drawn < depth ? drawn : depth;
        octree_fill_grid_stop(neighbors[d], sizeof(VoxelNode), octree_type_offset_VoxelNode, 0, depth, int3_zero, origin,
            grid->voxels, grid->length, 0, stop);
        if (drawn <= depth) {
            continue;
        }
        // drawn finer (or open): each border cell against its finer voxels
        const byte axis = d / 2, positive = d % 2;
        const byte a = axis == 0 ? 1 : 0, b = axis == 2 ? 1 : 2;
        const int k = open ? 1 : 1 << (drawn - depth);
        for (int u = 0; u < size; u++) for (int v = 0; v < size; v++) {
            int cell[3];
            cell[axis] = positive ? size : -1;
            cell[a] = u;
            cell[b] = v;
            byte* value = &grid->voxels[voxel_grid_index(grid->length, cell[0], cell[1], cell[2])];
            if (!*value) {
                continue;
            }
            byte air = open;
            if (!open) {
                // a closed node there is one value along the whole face: the fill already has it
                int coarse[3];
                coarse[axis] = positive ? 0 : size - 1;
                coarse[a] = u;
                coarse[b] = v;
                const VoxelNode* node = get_VoxelNode(neighbors[d], depth, (byte3) { coarse[0], coarse[1], coarse[2] });
                if (node && !has_children_VoxelNode(node)) {
                    *value = hides[node->value] ? *value : 0;
                    continue;
                }
            }
            for (int fu = 0; fu < k && !air; fu++) for (int fv = 0; fv < k && !air; fv++) {
                int fine[3];
                fine[axis] = positive ? 0 : size * k - 1;
                fine[a] = u * k + fu;
                fine[b] = v * k + fv;
                air = !hides[getv_VoxelNode(neighbors[d], drawn, (byte3) { fine[0], fine[1], fine[2] })];
            }
            if (air) {
                *value = 0;
            }
        }
    }
}

// which voxels a material's mesh draws and which hide its faces (block_material_hides), by voxel value, from a block
//  materials table (get_block_materials)
static inline void voxel_grid_material(const byte* materials, byte material, byte* drawn, byte* hides) {
    for (int v = 0; v < 256; v++) {
        drawn[v] = v && materials[v] == material;
        hides[v] = v && block_material_hides(materials[v], material);
    }
}

// the faces a grid voxel shows, a bit a direction, 0 if its mesh doesn't draw it: beside a voxel that doesn't hide
//  it, into a missing chunk only its top; i its cell (voxel_grid_index)
static inline byte voxel_grid_shows(const voxel_grid* grid, int size, int i, int x, int y, int z, const byte* drawn,
    const byte* hides)
{
    const byte* voxels = grid->voxels;
    const byte voxel = voxels[i];
    if (!voxel || !drawn[voxel]) {
        return 0;
    }
    // a neighbour's cell from a voxel's, by direction (left, right, down, up, back, front)
    const int length = grid->length;
    const int step[6] = { -length * length, length * length, -length, length, -1, 1 };
    byte shows = 0;
    for (byte d = 0; d < 6; d++) {
        shows |= !hides[voxels[i + step[d]]] << d;
    }
    if (x == 0 || y == 0 || z == 0 || x == size - 1 || y == size - 1 || z == size - 1) {
        const byte edge[6] = { x == 0, x == size - 1, y == 0, y == size - 1, z == 0, z == size - 1 };
        for (byte d = 0; d < 6; d++) {
            if (edge[d] && !grid->present[d]) {
                shows = (shows & ~(1 << d)) | ((d == direction_up) << d);
            }
        }
    }
    return shows;
}

// a chunk's grid at depth for a material's mesh (hides: voxel_grid_material): its 6 neighbours at the depths they draw
//  (chunk_side_depth, open while unsettled), every tree locked while it's read (zox_lock_voxel_trees); buffer is the
//  system's, voxel_grid_cells(depth) big
static inline void chunk_voxel_grid(ecs* world, entity chunk, byte depth, const byte* hides, byte* buffer, voxel_grid* grid) {
    const ChunkNeighbors* neighbors = zox_get(chunk, ChunkNeighbors);
    const VoxelNode* neighbor_voxels[6];
    spinlock* neighbor_locks[6];
    byte neighbor_depths[6];
    for (byte j = 0; j < 6; j++) {
        const entity neighbor = neighbors->value[j];
        neighbor_voxels[j] = NULL;
        neighbor_locks[j] = NULL;
        neighbor_depths[j] = 0;
        if (!zox_valid(neighbor) || !zox_has(neighbor, VoxelNode) || !zox_has(neighbor, NodeDepth)) {
            continue;
        }
        neighbor_depths[j] = chunk_side_depth(world, neighbor);
        neighbor_voxels[j] = zox_get(neighbor, VoxelNode);
        neighbor_locks[j] = &(zox_mut(neighbor, VoxelNodeLock)->value);
    }
    spinlock* locked[7];
    const byte locked_count = zox_lock_voxel_trees(&(zox_mut(chunk, VoxelNodeLock)->value), neighbor_locks, locked);
    voxel_grid_fill(grid, buffer, zox_get(chunk, VoxelNode), neighbor_voxels, neighbor_depths, depth, hides);
    zox_unlock_voxel_trees(locked, locked_count);
}

// a system's grid buffer grown to fit depth (it keeps one for its run, freed at its end)
static inline byte* voxel_grid_buffer(byte* buffer, size_t* size, byte depth) {
    const size_t cells = voxel_grid_cells(depth);
    if (*size < cells) {
        free(buffer);
        buffer = malloc(cells);
        *size = buffer ? cells : 0;
    }
    return buffer;
}

// the faces of a material from a filled grid, into data's mesh: drawn[v] is 1 for the material's voxels, hides[v] for
//  the voxels hiding its faces (0 air); scale is the chunk's size, as build_textured.c's chunk_scale
static inline void build_textured_grid(const voxel_grid* grid, terrain_build_data data, byte depth, float scale,
    const byte* drawn, const byte* hides)
{
    const int size = octree_size(depth);
    const int length = grid->length;
    const float voxel_scale = scale / size;
    for (int x = 0; x < size; x++) for (int y = 0; y < size; y++) {
        int i = voxel_grid_index(length, x, y, 0);
        for (int z = 0; z < size; z++, i++) {
            const byte shows = voxel_grid_shows(grid, size, i, x, y, z, drawn, hides);
            if (!shows) {
                continue;
            }
            const byte block = grid->voxels[i] - 1;
            const float3 positionf = (float3) { x * voxel_scale, y * voxel_scale, z * voxel_scale };
            const byte eighths = data.heights[block];
            const byte lowered = eighths && ((shows >> direction_up) & 1);
            float3 lowered_vertices[4];
            for (byte d = 0; d < 6; d++) {
                if (!((shows >> d) & 1)) {
                    continue;
                }
                const float3* vertices = voxel_face_vertices_n[d];
                if (lowered) {
                    for (byte k = 0; k < 4; k++) {
                        lowered_vertices[k] = vertices[k];
                        if (lowered_vertices[k].y > 0) {
                            lowered_vertices[k].y = eighths / (float) zox_water_level_source;
                        }
                    }
                    vertices = lowered_vertices;
                }
                zox_build_voxel_face(data.mesh_data, voxel_face_indicies_n + d * voxel_face_indicies_length, vertices,
                    data.whole_texture ? voxel_face_uvs[d] : &data.tilemap_uvs->value[block * 24 + d * 4],
                    positionf, float3_single(voxel_scale));
            }
        }
    }
}
