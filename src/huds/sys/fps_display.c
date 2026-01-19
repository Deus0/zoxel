zox_sys2(FpsDisplaySystem) {
    const double frame_rate_update_speed = 1.0;
    const byte number_0_start = 60;
    init_delta_time();
    zox_sys_begin();
    zox_sys_out(TextData);
    zox_sys_out(TextDirty);
    zox_sys_out(FPSDisplayTicker);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(TextDirty, zextDirty);
        zox_sys_o(FPSDisplayTicker, tick);
        zox_sys_o(TextData, data);

        if (zextDirty->value) {
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
        resize_memory_component(TextData, data, byte, 3)
        if (frames_per_second < 10) {
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
        }
        zextDirty->value = 1;
    }
} zox_sys_end(FpsDisplaySystem);