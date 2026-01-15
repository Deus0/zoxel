// Streamer components added to cameras
zox_sys2(StreamPointSystem) {
    if (zox_cameras_disable_streaming) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(BlockScale);
    zox_sys_in(NodeDepth);
    zox_sys_out(StreamPoint);
    zox_sys_out(StreamPoint2);
    zox_sys_out(StreamDirty);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_i(VoxLink, terrain);
        zox_sys_i(Position3D, position);
        zox_sys_i(BlockScale, scale);
        zox_sys_i(NodeDepth, depth);
        zox_sys_o(StreamPoint, point3);
        zox_sys_o(StreamPoint2, point2);
        zox_sys_o(StreamDirty, dirty);

        if (dirty->value) {
            continue;
        }

        int3 npoint = real_position_to_chunk_position(
            position->value,
            powers_of_two[depth->value],
            scale->value
        );
        if (int3_equals(npoint, point3->value)) {
            continue;
        }

        point3->value = npoint;
        point2->value = (int2) { npoint.x, npoint.z };
        dirty->value = zox_dirty_trigger;

        zox_log("Streaming Dirty: %ix%ix%i", npoint.x, npoint.y, npoint.z);
    }
} zox_sys_end(StreamPointSystem);