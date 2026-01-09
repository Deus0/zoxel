// Streamer components added to cameras
zox_sys2(StreamPointSystem) {
    if (zox_cameras_disable_streaming) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(VoxLink);
    zox_sys_out(StreamPoint);
    zox_sys_out(StreamDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(VoxLink, terrain);
        zox_sys_i(Position3D, position);
        zox_sys_o(StreamDirty, stream_dirty);
        zox_sys_o(StreamPoint, stream_point);

        if (stream_dirty->value) {
            continue;
        }

        if (!zox_valid(terrain->value) || !zox_has(terrain->value, BlockScale) || !zox_has(terrain->value, NodeDepth)) {
            continue;
        }

        zox_geter_value(terrain->value, BlockScale, float, terrain_scale);
        zox_geter_value(terrain->value, NodeDepth, byte, node_depth);

        const int3 new_point = real_position_to_chunk_position(
            position->value,
            powers_of_two[node_depth],
            terrain_scale
        );
        if (!int3_equals(new_point, stream_point->value)) {
            stream_point->value = new_point;
            stream_dirty->value = zox_dirty_trigger;
        }

    }
} zox_sys_end(StreamPointSystem);