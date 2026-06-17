// If Streamer2 is Dirty:
//  - Update Render Distances
//  - Update Render Depths
zox_sys2(TunkLodSystem) {
    if (zox_cameras_disable_streaming) {
        return;
    }
    zox_sys_query();
    zox_sys_world();
    byte any_dirty = 0;
    zox_sys_query_begin();
    while (zox_sys_query_loop()) {
        if (any_dirty) {
            continue;
        }
        zox_sys_begin_2();
        zox_sys_in_2(StreamDirty2);
        for (int j = 0; j < it2.count; j++) {
            zox_sys_i_2(StreamDirty2, sdirty);
            if (sdirty->value == zox_dirty_active) {
                any_dirty = 1;
                break;
            }
        }
    }
    zox_sys_query_end();
    if (!any_dirty) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(TunkPosition);
    zox_sys_out(RenderDistance);
    zox_sys_out(RenderDepth);
    zox_sys_out(RenderDistanceDirty);
    zox_sys_out(RenderDepthDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TunkPosition, position);
        zox_sys_o(RenderDistance, distance);
        zox_sys_o(RenderDepth, depth);
        zox_sys_o(RenderDistanceDirty, distance_dirty);
        zox_sys_o(RenderDepthDirty, depth_dirty);
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            continue;
        }
        int2 closest_point;
        byte had_streamer = 0;
        float closest_distance = 9999;
        zox_sys_query_begin();
        while (zox_sys_query_loop()) {
            zox_sys_begin_2_at(1);
            zox_sys_in_2(StreamerLevel);
            zox_sys_in_2(StreamLink);
            zox_sys_in_2(StreamPosition2);
            for (int j = 0; j < it2.count; j++) {
                zox_sys_i_2(StreamerLevel, level);
                zox_sys_i_2(StreamLink, stream_terrain);
                zox_sys_i_2(StreamPosition2, stream_point);
                if (level->value < 1) {
                    continue;
                }
                if (stream_terrain->value != terrain) {
                    continue;
                }
                float distance = int2_distance(stream_point->value, position->value);
                if (distance < closest_distance) {
                    had_streamer = 1;
                    closest_distance = distance;
                    closest_point = stream_point->value;
                }
            }
        }
        zox_sys_query_end();
        if (!had_streamer) {
            continue;
        }
        byte streamer_distance = get_camera_chunk2_distance(closest_point, position->value);
        if (distance->value != streamer_distance) {
            distance->value = streamer_distance;
            distance_dirty->value = zox_dirty_trigger;
            byte rdepth = camera_distance_to_terrain_render_depth(distance->value);
            if (depth->value != rdepth) {
                depth->value = rdepth;
                depth_dirty->value = zox_dirty_trigger;
            }
        }
    }
} zox_sys_end(TunkLodSystem);
