// Streamer components added to cameras
zox_sys2(StreamPointSystem) {
    if (zox_cameras_disable_streaming) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StreamLink);
    zox_sys_in(Position3D);
    // zox_sys_in(BlockScale);
    // zox_sys_in(NodeDepth);
    zox_sys_out(StreamPoint);
    zox_sys_out(StreamPoint2);
    zox_sys_out(StreamDirty);
    zox_sys_out(StreamDirty2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(StreamLink, terrain);
        zox_sys_i(Position3D, position);
        // zox_sys_i(BlockScale, scale);
        // zox_sys_i(NodeDepth, depth);
        zox_sys_o(StreamPoint, point3);
        zox_sys_o(StreamPoint2, point2);
        zox_sys_o(StreamDirty, dirty);
        zox_sys_o(StreamDirty2, dirty2);

        if (dirty->value) {
            continue;
        }

        // Get Terrain Data
        if (!zox_valid(terrain->value) || !zox_has(terrain->value, BlockScale) || !zox_has(terrain->value, NodeDepth)) {
            continue;
        }

        zox_geter_value(terrain->value, BlockScale, float, tscale);
        zox_geter_value(terrain->value, NodeDepth, byte, ndepth);

        int3 npoint = real_position_to_chunk_position(
            position->value,
            powers_of_two[ndepth],
            tscale
        );

        if (int3_equals(npoint, point3->value)) {
            continue;
        }

        point3->value = npoint;
        dirty->value = zox_dirty_trigger;

        int2 npoint2 = (int2) { npoint.x, npoint.z };
        if (!int2_equals(point2->value, npoint2)) {
            point2->value = npoint2;
            dirty2->value = zox_dirty_trigger;
        }

        // zox_log("Streaming Dirty: %ix%ix%i", npoint.x, npoint.y, npoint.z);
    }
} zox_sys_end(StreamPointSystem);