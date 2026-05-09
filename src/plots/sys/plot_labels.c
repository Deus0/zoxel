zox_sys2(PlotLabelSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(TextDirty);
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(TextDirty, dirty);
        zox_sys_o(TextData, textData);
        if (dirty->value) {
            continue;
        }
        entity parent = zox_get_parent(world, e);
        if (!zox_valid(parent)) {
            continue;
        }
        if (!zox_has(parent, PlotMin) || !zox_has(parent, PlotMax)) {
            zox_log_error("invalid plot");
            continue;
        }
        zox_geter_value(parent, PlotMin, double, min);
        zox_geter_value(parent, PlotMax, double, max);
        byte buffer_size = 128;
        int buffer_index = 0;
        char buffer[buffer_size];
        buffer_index += snprintf(buffer + buffer_index, buffer_size - buffer_index, "Min %.0fms\nMax %.0fms\n", min, max);
        if (!is_zext(textData, buffer)) {
            set_zext(textData, buffer);
            dirty->value = 1;
        }
    }
} zox_sys_end(PlotLabelSystem);
