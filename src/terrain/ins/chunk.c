entity spawn_chunk3_terrain(ecs* world, entity prefab, entity terrain, int3 position, byte terrain_depth, float terrain_scalev, byte render_distance, byte render_depth) {
    // byte camera_distance = get_camera_chunk_distance_xz(camera_position, position);
    // byte render_depth = camera_distance_to_terrain_render_depth(camera_distance);
    zox_instance(prefab);
    char name[64];
    sprintf(name, "chunk_terrain_%ix%ix%i", position.x, position.y, position.z);
    // zox_name("chunk_terrain");
    zox_name(name);
    zox_set_parent(world, e, terrain);
    // convert chunk position to real
    //  - scales by length of chunk and vox scale
    zox_set(e, VoxLink, { terrain });
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
    zox_set(e, BlockScale, { chunk_scalev });    // set from parent
    // we should just pass in positionf - local position of parent!
    byte length = octree_size(terrain_depth);
    float3 positionf = float3_scale(float3_from_int3(position), length * terrain_scalev);
    zox_set(e, ChunkSize, { int3_single(length) });
    // zox_log("placing: chunk_scale [%f] voxscale [%f] length [%i]", chunk_scale, scale, length);
    zox_set(e, Position3D, { positionf });
    zox_set(e, TransformMatrix, { float4x4_position(positionf) });
    // lod update here
    // todo: just start this as invisible and update with streaming systems
    zox_set(e, RenderDistance, { render_distance });
    zox_set(e, RenderDistanceDirty, { zox_dirty_trigger });
    zox_set(e, RenderDepth, { render_depth });
    /*if (position.y == render_distance_y) {
        zox_add_tag(e, SunnyChunk);
    } else if (position.y == -render_distance_y) {
        zox_add_tag(e, BottomChunk);
    }*/
    // can move this to init systems
    //  note: keep spawn functions only for passing through sending outside information
    // NOTE: Move this to a system - in Rendering - initialize render
    spawn_gpu_mesh(world, e);
    spawn_gpu_uvs(world, e);
    spawn_gpu_colors(world, e);
    return e;
}
