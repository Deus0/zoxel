// If Streamer2 is Dirty:
//  - Update Render Distances
//  - Update Render Depths
zox_sys2(Chunk2LodSystem) {
    if (zox_cameras_disable_streaming) {
        return;
    }
    zox_sys_query();
    zox_sys_world();
    byte dirty = 0;
    int2* streamers = NULL;
    int streamers_count = 0;
    zox_sys_query_begin();
    while (zox_sys_query_loop()) {
        if (dirty) {
            continue;
        }
        zox_sys_begin_2();
        zox_sys_in_2(StreamPoint2);
        zox_sys_in_2(StreamDirty2);
        for (int j = 0; j < it2.count; j++) {
            zox_sys_i_2(StreamDirty2, sdirty);
            if (sdirty->value == zox_dirty_active) {
                streamers = (int2*) StreamPoint2s_2;
                streamers_count = it2.count;
                dirty = 1;
                break;
            }
        }
    }
    zox_sys_query_end()

    if (!dirty) {
        return;
    }

    zox_sys_begin();
    zox_sys_in(Chunk2Position);
    zox_sys_out(RenderDistance);
    zox_sys_out(RenderDepth);
    zox_sys_out(RenderDistanceDirty);
    zox_sys_out(RenderDepthDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Chunk2Position, position);
        zox_sys_o(RenderDistance, distance);
        zox_sys_o(RenderDepth, depth);
        zox_sys_o(RenderDistanceDirty, distance_dirty);
        zox_sys_o(RenderDepthDirty, depth_dirty);

        int2 stream_point = find_closest_point2(streamers, streamers_count, position->value);

        byte rdistance = get_camera_chunk2_distance(stream_point, position->value);

        if (distance->value != rdistance) {
            distance->value = rdistance;
            distance_dirty->value = zox_dirty_trigger;

            byte rdepth = camera_distance_to_terrain_render_depth(distance->value);
            if (depth->value != rdepth) {
                depth->value = rdepth;
                if (rdepth != render_depth_invisible) {
                    depth_dirty->value = zox_dirty_trigger;
                }
            }
        }
    }
} zox_sys_end(Chunk2LodSystem);
