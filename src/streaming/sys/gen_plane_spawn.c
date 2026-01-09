zox_sys_begin(GenPlaneSpawnSystem) {
    const entity prefab_chunk = prefab_chunk_terrain;

    zox_sys_query();
    zox_sys_world();

    const byte log_individuals = 0;
    uint spawned_chunks = 0;
    int3 *stream_points = NULL;
    int stream_points_length = 0;
    byte iterated = 0;

    zox_sys_query_begin();
    while (zox_sys_query_loop()) {
        if (!iterated) {
            iterated = 1;
            zox_sys_begin_2();
            zox_sys_in_2(StreamPoint);
            stream_points = (int3*) StreamPoints_2;
            stream_points_length = it2.count;
        }
    }

    zox_sys_query_end();
    if (stream_points_length == 0) {
        return;
    }

    zox_sys_begin();
    zox_sys_in(ChunkPosition);
    zox_sys_in(VoxLink);
    zox_sys_in(RenderDistance);
    zox_sys_out(ChunkNeighbors);

    for (int i = 0; i < it->count; i++) {

        // zox_sys_e();
        zox_sys_i(VoxLink, voxLink);
        zox_sys_i(RenderDistance, renderDistance);
        zox_sys_i(ChunkPosition, chunkPosition);
        zox_sys_o(ChunkNeighbors, chunkNeighbors);

        if (!zox_valid(voxLink->value)) {
            continue;
        }

        // Pass if loading chunk
        if (renderDistance->value == 255) {
            continue;
        }

        zox_geter_value(voxLink->value, BlockScale, float, terrain_scale);
        const byte stream_zone = renderDistance->value < terrain_lod_far;
        if (stream_zone) {

        }
    }

    if (spawned_chunks > 0) {
        zox_log_streaming(" + [%i] spawned [%i]", ecs_run_count, spawned_chunks);
    }

} zox_sys_end(GenPlaneSpawnSystem);
