entity spawn_terrain_chunk(ecs* world, entity prefab, entity terrain, int3 position, byte terrain_depth, float terrain_scalev, byte render_distance, byte render_depth) {
    entity e = zox_ins(world, prefab);
    {
        char name[64];
        sprintf(name, "terrain_chunk_%ix%ix%i", position.x, position.y, position.z);
        zox_name(name);
    }
    zox_set_parent(world, e, terrain);
    // convert chunk position to real
    //  - scales by length of chunk and vox scale
    // zox_set(e, VoxLink, { terrain });
    zox_set(e, ChunkPosition, { position });
    zox_geter_value(terrain, TilemapLink, entity, tilemap);
    if (!tilemap) {
        zox_log_error("Tilemap on terrain null.");
    } else {
        zox_set(e, TilemapLink, { tilemap });
    }
    zox_geter_value(terrain, RealmLink, entity, realm);
    if (!zox_valid(realm)) {
        zox_logw("realm not on terrain");
    } else {
        zox_set(e, RealmLink, { realm });
        zox_set(e, BlockManagerLink, { realm });
    }
    // scale needs to be based on chunk itself
    float chunk_scalev = get_chunk_scale(render_depth, terrain_depth, terrain_scalev);
    // we should just pass in positionf - local position of parent!
    byte length = octree_size(terrain_depth);
    int3 size = int3_single(length);
    float3 bounds = calculate_vox_bounds(size, chunk_scalev);
    float3 positionf = float3_scale(float3_from_int3(position), length * terrain_scalev);
    zox_set(e, BlockScale, { chunk_scalev });    // set from parent
    zox_set(e, ChunkSize, { size });
    zox_set(e, Bounds3D, { bounds });
    // zox_log("placing: chunk_scale [%f] voxscale [%f] length [%i]", chunk_scale, scale, length);
    // NOTE: Used in ChunkFrustumCulling for now
    zox_set(e, Position3D, { positionf });
    // Used for Rendering
    zox_set(e, TransformMatrix, { float4x4_position(positionf) });
    // zox_set(e, TransformMatrix, { float4x4_transform_scale(positionf, quaternion_identity, 1) });
    // lod update here
    // todo: just start this as invisible and update with streaming systems
    zox_set(e, RenderDistance, { render_distance });
    zox_set(e, RenderDepth, { render_depth });
    return e;
}

// can move this to init systems
//  note: keep spawn functions only for passing through sending outside information
// NOTE: Move this to a system - in Rendering - initialize render
/*if (zox_has(prefab, MeshIndicies)) {
 * spawn_gpu_mesh(world, e);
 * spawn_gpu_uvs(world, e);
 * spawn_gpu_colors(world, e);
 } */
// zox_set(e, TransformMatrix, { float4x4_position_scale(positionf, 1) });
// zox_set(e, TransformMatrix, { float4x4_transform_scale(positionf, quaternion_identity, 1) });
// zox_set(e, TransformMatrix, { float4x4_transform(positionf, quaternion_identity) });
