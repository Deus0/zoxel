entity spawn_chunk_terrain(ecs *world, entity p, entity terrain, int3 camera_position, int3 position, byte terrain_depth, float terrain_scalev) {

    zox_instance(p);
    zox_name("chunk_terrain");

    if (terrain_mode == terrain_mode_flatlands) {
        zox_add_tag(e, FlatlandChunk);
    }

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
    if (!realm) {
        zox_logw("realm not on terrain");
    } else {
        zox_set(e, BlockManagerLink, { realm });
    }

    // scale needs to be based on chunk itself
    const byte camera_distance = get_camera_chunk_distance_xz(camera_position, position);
    const byte render_depth = camera_distance_to_terrain_render_depth(camera_distance);
    const float chunk_scalev = get_chunk_scale(
        render_depth,
        terrain_depth,
        terrain_scalev
    );
    zox_set(e, BlockScale, { chunk_scalev });    // set from parent

    // we should just pass in positionf - local position of parent!
    byte terrain_length = powers_of_two[terrain_depth];
    const float3 positionf = float3_scale(
        float3_from_int3(position),
        terrain_length * terrain_scalev
    );
    zox_set(e, ChunkSize, { int3_single(terrain_length) });

    // zox_log("placing: chunk_scale [%f] voxscale [%f] terrain_length [%i]", chunk_scale, scale, terrain_length);

    zox_set(e, Position3D, { positionf });
    zox_set(e, TransformMatrix, { float4x4_position(positionf) });

    // lod update here
    // todo: just start this as invisible and update with streaming systems
    zox_set(e, RenderDistance, { camera_distance });
    zox_set(e, RenderDistanceDirty, { zox_dirty_trigger });

    // zox_set(e, RenderDepthMax, { terrain_depth });
    zox_set(e, RenderDepth, { render_depth });
    if (render_depth != render_depth_invisible) {
        // zox_set(e, RenderDepthDirty, { zox_dirty_trigger });
    }

    if (position.y == render_distance_y) {
        zox_add_tag(e, SunnyChunk);
    }

    // can move this to init systems
    //  note: keep spawn functions only for passing through sending outside information
    if (!headless) {
        spawn_gpu_mesh(world, e);
        spawn_gpu_uvs(world, e);
        spawn_gpu_colors(world, e);
    }

    return e;
}
