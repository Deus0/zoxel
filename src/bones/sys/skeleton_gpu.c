// Gpu meshing of skeleton meshes (doc/plans/chunks_refactor.md phase 4): a GpuMesh SkeletonMesh (zox_vox_gpu_meshing)
//  is meshed, coloured and weighted by the compute shader (chunks3/sys/vox_gpu.c) in one go: its vertices' two bones
//  and weight by generate.c's rule (bone_parts_vertex) from its vox's bones grid, into its BoneIndexGPULink
//  - BuildMesh: voxels changed, or its skeleton did (bone_index_generate_system asks for it instead of the weights)
//  - it waits while its vox's voxels are changing (its parts table may be newer than them, generate.c)
//  - then MeshBuilt (greedy: once its merged count is back): skeleton_meshes_system swaps it in

// its vox's bones at depth in gpu_mesh_grid's layout: each cell's bone (bone_parts_most), 255 air and round it
static byte* skeleton_gpu_bones_grid(ecs* world, entity vox, byte depth, const void* data) {
    (void) data;
    const size_t cells = gpu_mesh_grid_bytes(depth);
    byte* grid = malloc(cells);
    memset(grid, 255, cells);
    if (!zox_has(vox, BoneNode) || !zox_has(vox, CombineBones)) {
        return grid;
    }
    const int size = octree_size(depth);
    const int length = size + 2;
    byte* bones = malloc(size * size * size);
    bone_parts_fill(bones, size, zox_get(vox, CombineBones), zox_get(vox, BoneNode), 0, depth, int3_zero);
    for (int x = 0; x < size; x++)
    for (int y = 0; y < size; y++) {
        memcpy(grid + ((x + 1) * length + y + 1) * length + 1, bones + (x * size + y) * size, size);
    }
    free(bones);
    return grid;
}

// bones off: every cell air, every vertex the first bone's (as generate_weights' zeroes)
static byte* skeleton_gpu_no_bones(ecs* world, entity vox, byte depth, const void* data) {
    (void) world;
    (void) vox;
    (void) data;
    const size_t cells = gpu_mesh_grid_bytes(depth);
    byte* grid = malloc(cells);
    memset(grid, 255, cells);
    return grid;
}

void skeleton_mesh_gpu_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_in(MeshGPULink);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(BoneIndexGPULink);
    zox_sys_out(MeshFaceRoom);
    zox_sys_out(MeshRenderCount);
    zox_sys_out(MeshVertices);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, depth);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(ColorsGPULink, colors);
        zox_sys_i(BoneIndexGPULink, weights);
        zox_sys_o(MeshFaceRoom, room);
        zox_sys_o(MeshRenderCount, count);
        zox_sys_o(MeshVertices, vertices);
        if (!mesh->value.x || !mesh->value.y || !colors->value || !weights->value) {
            continue;   // its buffers come with its initialize
        }
        const entity skeleton = zox_get_parent(world, e);
        const entity vox = get_skeleton_vox(world, skeleton);
        // a stale mesh (its voxels changed, a new one building) keeps its old faces and weights
        if (zox_has(e, SkeletonMeshStale)) {
            zox_remove(e, BuildMesh);
            continue;
        }
        if (!zox_valid(vox) || !zox_has(skeleton, BoneLinks) || zox_has(vox, VoxelNodeDirty) ||
            zox_has(vox, VoxelNodePostDirty) || zox_has(vox, ChunkUpdate) ||
            (zox_has(vox, ColorRGBs) && !zox_get(vox, ColorRGBs)->length))
        {
            continue;
        }
        if (vox_gpu_budget_spent()) {
            continue;
        }
        const double begin = get_time_ms();
        const BoneLinks* bones = zox_get(skeleton, BoneLinks);
        vox_gpu_skin skin = { .weights = weights->value, .bones_count = bones->length, .grid = skeleton_gpu_bones_grid };
        if (!disable_bones && bones->length && bones->length <= 255) {
            byte parents[256];
            memset(parents, 255, sizeof(parents));
            bone_parents_fill(world, bones, parents);
            memcpy(skin.parents, parents, sizeof(skin.parents));
        } else {
            skin.bones_count = 0;
            skin.grid = skeleton_gpu_no_bones;
            memset(skin.parents, 255, sizeof(skin.parents));
        }
        float3 bounds[2];
        const uint faces = vox_gpu_mesh(world, vox, depth->value, zox_block_material_solid, mesh->value, colors->value,
            &room->value, bounds, &skin, vox_gpu_command_of(world, e), vox_gpu_fence_of(world, e));
        zox_vox_gpu_budget_used += get_time_ms() - begin;
        vox_gpu_meshed(world, e, faces, bounds, count, vertices);
        zox_remove(e, BuildMesh);
        // built now, or once its merged count is back (greedy, vox_gpu_count_system)
        if (!vox_gpu_pending(world, e)) {
            zox_add(e, MeshBuilt);
        }
    }
    zox_sys_on_end();
} zoxd_system(skeleton_mesh_gpu_system);
