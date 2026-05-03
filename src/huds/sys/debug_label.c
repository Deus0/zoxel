double time_update_debug_label_system_rate = 1.0;
uint max_debug_characters = 4 * 1024;
double time_update_debug_label_system = 0;

zox_sys2(DebugLabelSystem) {
    // timer function for label updates
    time_update_debug_label_system += zox_delta_time;
    if (time_update_debug_label_system >= time_update_debug_label_system_rate) {
        time_update_debug_label_system = 0;
    } else {
        return;
    }

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerLink);
    zox_sys_in(DebugLabelData);
    zox_sys_out(TextDirty);
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(PlayerLink, playerLink);
        zox_sys_i(DebugLabelData, debugLabelData);
        zox_sys_o(TextData, data);
        zox_sys_o(TextDirty, dirty);

        if (dirty->value) {
            continue;
        }

        entity player = playerLink->value;
        if (!player) {
            continue;
        }

        uint index = 0;
        uint size = max_debug_characters;
        char buffer[size];
        index += snprintf(buffer + index, size - index, "%s [v0.0.1]\n", game_name);

        if (debugLabelData->value) {
            debugLabelData->value(world, player, buffer, size, index);
        }

        // now finish it
        if (index == 0) {
            buffer[0] = '\0';
        }

        if (!is_zext(data, buffer)) {
            set_zext(data, buffer);
            dirty->value = 1;
        }
    }
} zox_sys_end(DebugLabelSystem);