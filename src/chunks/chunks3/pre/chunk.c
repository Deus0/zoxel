entity spawn_prefab_chunk(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("chunk");
    zox_prefab_set(e, BlockScale, { default_vox_scale });
    // Transforms
    zox_prefab_set(e, Position3D, { float3_zero });
    zox_prefab_set(e, Rotation3D, { float4_identity });
    zox_prefab_set(e, TransformMatrix, { float4x4_identity });
    // Rendering
    zox_prefab_set(e, Brightness, { 1.0f });
    zox_prefab_set(e, RenderDisabled, { 0 });
    zox_prefab_set(e, MeshDirty, { 0 });
    prefab_add_mesh_basic(world, e);
    return e;
}

// A simple colored vox at position
entity spawn_chunk3(ecs *world, entity prefab, float3 position, float scale, byte depth, float block_scale, int3 size) {
    /*byte terrain_length = powers_of_two[max_depth];
    float3 positionf = float3_scale(float3_from_int3(position), terrain_length * scale);
    byte camera_distance = get_camera_chunk_distance_xz(camera_position, position);
    byte render_depth = camera_distance_to_terrain_render_depth(camera_distance);
    float chunk_scalev = get_chunk_scale(depth, max_depth, scale);*/
    // Spawn
    zox_instance(prefab);
    zox_name("chunk3")
    zox_set(e, Position3D, { position });
    zox_set(e, Scale1D, { scale });
    zox_set(e, BlockScale, { block_scale });
    // zox_set(e, TransformMatrix, { float4x4_position(position) });
    // zox_set(e, ChunkPosition, { position });
    zox_set(e, ChunkSize, { size });
    zox_set(e, RenderDepth, { depth });
    // zox_set(e, RenderDistance, { camera_distance });
    // zox_set(e, RenderDistanceDirty, { zox_dirty_trigger });
    // zox_set(e, VoxLink, { terrain });
    spawn_gpu_mesh(world, e);
    spawn_gpu_colors(world, e);
    zox_add_tag(e, NoiseChunk);
    zox_set(e, Generate, { zox_dirty_trigger });
    return e;
}
