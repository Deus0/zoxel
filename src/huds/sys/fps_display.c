zox_sys2(FpsDisplaySystem) {
    byte dbg_log = 0;
    double frame_rate_update_speed = 1.0;
    // byte number_0_start = 60;
    init_delta_time();
    zox_sys_begin();
    zox_sys_out(TextData);
    zox_sys_out(TextDirty);
    zox_sys_out(FPSDisplayTicker);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(TextDirty, dirty);
        zox_sys_o(FPSDisplayTicker, tick);
        zox_sys_o(TextData, data);
        if (dirty->value) {
            continue;
        }
        tick->value -= delta_time;
        if (tick->value > 0) {
            continue;
        }
        tick->value += frame_rate_update_speed;
        if (tick->value <= -frame_rate_update_speed) {
            tick->value = 0;
        }
        char buffer[8];
        snprintf(buffer, 8, "%i\n", frames_per_second);
        if (!is_zext(data, buffer)) {
            set_zext(data, buffer);
            dirty->value = zox_dirty_trigger;
            if (dbg_log) {
                zox_log("FPS updated [%s]", buffer);
            }
        }
    }
} zox_sys_end(FpsDisplaySystem);

        /*if (frames_per_second < 10) {
            data->value[0] = number_0_start;
            data->value[1] = number_0_start;
            data->value[2] = number_0_start + frames_per_second;
        } else if (frames_per_second < 100) {
            data->value[0] = number_0_start;
            data->value[1] = number_0_start + (frames_per_second / 10);
            data->value[2] = number_0_start + (frames_per_second % 10);
        } else if (frames_per_second < 1000) {
            data->value[0] = number_0_start + (frames_per_second / 100);
            data->value[1] = number_0_start + (frames_per_second / 10) % 10;
            data->value[2] = number_0_start + ((frames_per_second % 100) % 10);
        } else {
            data->value[0] = number_0_start;
            data->value[1] = number_0_start;
            data->value[2] = number_0_start;
        }*/