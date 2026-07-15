// Streamer components added to cameras
zox_sys2(StreamPositionSystem) {
    if (zox_cameras_disable_streaming) {
        return;
    }
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StreamLink);
    zox_sys_in(Position3D);
    zox_sys_out(StreamPosition);
    zox_sys_out(StreamPosition2);
    zox_sys_out(StreamDirty);
    zox_sys_out(StreamDirty2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(StreamLink, terrain);
        zox_sys_i(Position3D, position);
        zox_sys_o(StreamPosition, point3);
        zox_sys_o(StreamPosition2, point2);
        zox_sys_o(StreamDirty, dirty);
        zox_sys_o(StreamDirty2, dirty2);
        if (dirty->value) {
            continue;
        }
        if (!zox_valid(terrain->value) || !zox_has(terrain->value, BlockScale) || !zox_has(terrain->value, NodeDepth)) {
            continue;
        }
        zox_geter_value(terrain->value, BlockScale, float, tscale);
        zox_geter_value(terrain->value, NodeDepth, byte, ndepth);
        short length = octree_size(ndepth);
        int3 npoint = real_position_to_chunk_position(position->value, length, tscale);
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
        if (dbg_log) {
            zox_log("Streaming Dirty: %ix%ix%i", npoint.x, npoint.y, npoint.z);
        }
    }
} zox_sys_end(StreamPositionSystem);
