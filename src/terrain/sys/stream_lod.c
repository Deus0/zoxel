// this passes cameras into chunks
// each chunk will calculate distance to nearest camera and based LOD off this distance
// If Streamer is Dirty:
//  - Update Render Distances
//  - Update Render Depths
/*zox_sys2(TerrainChunkLodSystem) {
    if (zox_cameras_disable_streaming) {
        return;
    }
    zox_sys_query();
    zox_sys_world();
    byte any_dirty = 0;
    // TODO: Check per chunk as they might have different terrains
    zox_sys_query_begin();
    while (zox_sys_query_loop()) {
        if (any_dirty) {
            continue;
        }
        zox_sys_begin_2();
        zox_sys_in_2(StreamDirty);
        zox_sys_in_2(StreamerLevel);
        for (int j = 0; j < it2.count; j++) {
            zox_sys_i_2(StreamDirty, dirty);
            zox_sys_i_2(StreamerLevel, level);
            if (dirty->value != zox_dirty_active) {
                continue;
            }
            if (level->value < 1) {
                continue;
            }
            any_dirty = 1;
            break;
        }
    }
    zox_sys_query_end()
    if (!any_dirty) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(ChunkPosition);
    zox_sys_out(RenderDepth);
    zox_sys_out(RenderDistance);
    zox_sys_out(RenderDepthDirty);
    zox_sys_out(RenderDistanceDirty);
    zox_sys_out(Busy);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ChunkPosition, position);
        zox_sys_o(RenderDepth, render_depth);
        zox_sys_o(RenderDistance, distance);
        zox_sys_o(RenderDepthDirty, depth_dirty);
        zox_sys_o(RenderDistanceDirty, distance_dirty);
        zox_sys_o(Busy, busy);
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            continue;
        }
        byte had_streamer = 0;
        float closest_distance = 9999;
        zox_sys_query_begin();
        while (zox_sys_query_loop()) {
            zox_sys_begin_2_at(1);
            zox_sys_in_2(StreamerLevel);
            zox_sys_in_2(StreamLink);
            zox_sys_in_2(StreamPosition);
            for (int j = 0; j < it2.count; j++) {
                zox_sys_i_2(StreamerLevel, level);
                zox_sys_i_2(StreamLink, stream_terrain);
                zox_sys_i_2(StreamPosition, stream_point);
                if (level->value < 1) {
                    continue;
                }
                if (stream_terrain->value != terrain) {
                    continue;
                }
                // NOTE: The issue of sunlight was that highest chunk was less depth than ground one!
                // float distance = int3_distance(stream_point->value, position->value);
                float distance = int2_distance((int2) { stream_point->value.x, stream_point->value.z }, (int2) { position->value.x, position->value.z });
                if (distance < closest_distance) {
                    had_streamer = 1;
                    closest_distance = distance;
                    // closest_point = stream_point->value;
                }
            }
        }
        zox_sys_query_end();
        if (!had_streamer) {
            continue;
        }
        if (distance->value == closest_distance) {
            continue;
        }
        distance->value = closest_distance;
        distance_dirty->value = zox_dirty_trigger;
        byte new_depth = camera_distance_to_terrain_render_depth(distance->value);
        if (render_depth->value == new_depth) {
            continue;
        }
        render_depth->value = new_depth;
        depth_dirty->value = zox_dirty_trigger;
        busy->value = 1;
        // if new_depth > node_depth
        if (!zox_no_lights) {
            if (new_depth > zox_getv(e, NodeDepth)) {
                zox_muter(e, LightNode, lights);
                lights->value = darklight;
                collapse_LightNode(lights);
                if (zox_has(e, SunnyChunk)) {
                    zox_add(e, GenerateSunlight);
                }
            }
        }
    }
} zox_sys_end(TerrainChunkLodSystem);*/
