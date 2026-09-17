uint max_debug_characters = 6 * 1024;
double time_update_debug_label_system = 0;

zox_sys2(DebugLabelSystem) {
    byte dbg_log = 0;
    uint size = max_debug_characters;
    // timer function for label updates
    double update_rate = time_update_debug_label_system_rate;
    time_update_debug_label_system += zox_delta_time;
    if (time_update_debug_label_system >= update_rate) {
        time_update_debug_label_system = 0;
    } else {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DebugLabelData);
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DebugLabelData, debugger);
        zox_sys_o(TextData, data);
        entity player = zox_get_link(world, e, PlayerLink);
        if (!zox_valid(player)) {
            continue;
        }
        uint index = 0;
        char buffer[size];
        if (debugger->value) {
            index += debugger->value(world, player, buffer, size, index);
        } else {
            index += snprintf(buffer + index, size - index, "%s [v0.0.1]\n", game_name);
        }
        if (index == 0) {
            continue;
        }
        if (!is_zext(data, buffer)) {
            set_zext(data, buffer);
            zox_add(e, Dirty);
            if (dbg_log) {
                zox_log("Set Text to [%s]", buffer);
            }
        }
    }
} zox_sys_end(DebugLabelSystem);
