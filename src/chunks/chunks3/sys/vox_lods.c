// VoxLods: a single max detail vox, with a child VoxLodMesh per render depth
//  - like terrain chunks, the voxels live on the parent, the meshes are children
//  - each lod mesh builds from the parent octree at its own RenderDepth
//  - the octree optimizer (VoxelNodePostDirty) sets parent node values, which lower lods use

// When the vox voxels update, rebuild all of its lod meshes
void VoxLodsMeshTriggerSystem(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity mesh = it2.entities[j];
                if (!zox_has(mesh, VoxLodMesh)) {
                    continue;
                }
                zox_add(mesh, BuildMesh);
            }
        }
        if (dbg_log) {
            zox_log("VoxLods Triggered Lod Meshes to Build [%s]", zox_sys_e_name);
        }
    }
    zox_sys_on_end();
} zoxd_system(VoxLodsMeshTriggerSystem);

#define zox_vox_mesh_invalid 0
#define zox_vox_mesh_waiting 1
#define zox_vox_mesh_built 2

// Builds a colored mesh from a vox's max depth voxels, at a target depth
//  - used by meshes that are children of their vox (VoxLodMesh, SkeletonMesh)
byte build_vox_mesh_at_depth(
    ecs* world,
    entity vox,
    byte depth,
    MeshIndicies* indicies,
    MeshVertices* vertices,
    MeshColorRGBs* colors)
{
    if (!zox_valid(vox) ||
        !zox_has(vox, VoxelNode) ||
        !zox_has(vox, NodeDepth) ||
        !zox_has(vox, ColorRGBs) ||
        !zox_has(vox, ChunkSize) ||
        !zox_has(vox, BlockScale))
    {
        return zox_vox_mesh_invalid;
    }
    zox_geter(vox, ColorRGBs, vcolors);
    if (!vcolors->length) {
        // NOTE: Voxels not ready, it will be triggered again on VoxelNodeDirty
        return zox_vox_mesh_waiting;
    }
    zox_geter(vox, VoxelNode, voxels);
    byte node_depth = zox_getv(vox, NodeDepth);
    int3 chunk_size = zox_getv(vox, ChunkSize);
    float block_scale = zox_getv(vox, BlockScale);
    byte target_depth = depth;
    if (target_depth > node_depth) {
        target_depth = node_depth;
    }
    byte is_split = !zox_has(vox, NoSplitChunk);
    const VoxelNode* noctrees[6] = { 0 };
    const byte nrdepths[6] = { 0 };
    // NOTE: Same space for all lods, positions are based on the max depth vox
    ushort vlength = octree_size(node_depth);
    float3 bounds = calculate_vox_bounds(chunk_size, block_scale);
    float3 position = float3_scale(bounds, -1);
    float cscale = block_scale * vlength;
    clear_mesh(indicies, vertices, colors);
    mesh_colored_build_data mesh = {
        .indicies = create_int_array_d(initial_dynamic_array_size),
        .vertices = create_float3_array_d(initial_dynamic_array_size),
        .colors = create_color_rgb_array_d(initial_dynamic_array_size)
    };
    if (!is_split || !build_voxel_mesh_grid(
        voxels,
        vcolors,
        &mesh,
        target_depth,
        position,
        cscale))
    build_voxel_mesh_c(
        voxels,
        voxels,
        noctrees,
        nrdepths,
        vcolors,
        &mesh,
        target_depth,
        0,
        byte3_zero,
        position,
        cscale,
        is_split);
    indicies->length = mesh.indicies->size;
    vertices->length = mesh.vertices->size;
    colors->length = mesh.colors->size;
    indicies->value = finalize_arrayd_int(mesh.indicies);
    vertices->value = finalize_arrayd_float3(mesh.vertices);
    colors->value = finalize_arrayd_color_rgb(mesh.colors);
    return zox_vox_mesh_built;
}

// a few ms a worker a frame, the rest next frame (a realm's 216 lod meshes at once were a 275ms frame)
double zox_vox_mesh_budget_ms = 3.0;

// Builds a colored mesh from the parent VoxLods at RenderDepth
void VoxLodMeshBuildSystem(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_budget(vox_mesh, zox_disable_process_skips ? 1000000 : zox_vox_mesh_budget_ms);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepth);
    zox_sys_out(MeshIndicies);
    zox_sys_out(MeshVertices);
    zox_sys_out(MeshColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_o(MeshIndicies, indicies);
        zox_sys_o(MeshVertices, vertices);
        zox_sys_o(MeshColorRGBs, colors);
        if (zox_sys_budget_spent(vox_mesh)) {
            continue;
        }
        zox_sys_budget_start(vox_mesh);
        entity vox = zox_get_parent(world, e);
        byte result = build_vox_mesh_at_depth(
            world,
            vox,
            rdepth->value,
            indicies,
            vertices,
            colors);
        zox_sys_budget_end(vox_mesh);
        zox_remove(e, BuildMesh);
        if (result == zox_vox_mesh_invalid) {
            zox_loge("VoxLodMesh [%s] has invalid parent vox", zox_sys_e_name);
            continue;
        }
        if (result != zox_vox_mesh_built) {
            continue;
        }
        zox_add(e, MeshDirty);
        if (dbg_log) {
            zox_log("[%s] built lod mesh from [%s] depth [%i] tris [%i]",
                zox_sys_e_name,
                zox_getn(vox),
                rdepth->value,
                indicies->length / 3);
        }
    }
    zox_sys_on_end();
} zoxd_system(VoxLodMeshBuildSystem);
