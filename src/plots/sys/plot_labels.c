zox_sys2(PlotLabelSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ParentLink);
    zox_sys_out(TextDirty);
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ParentLink, parent);
        zox_sys_o(TextDirty, zextDirty);
        zox_sys_o(TextData, textData);

        if (zextDirty->value) {
            continue;
        }

        if (!zox_valid(parent->value)) {
            continue;
        }

        if (!zox_has(parent->value, PlotMin) || !zox_has(parent->value, PlotMax)) {
            zox_log_error("invalid plot");
            continue;
        }

        zox_geter_value(parent->value, PlotMin, double, min);
        zox_geter_value(parent->value, PlotMax, double, max);

        byte buffer_size = 128;
        int buffer_index = 0;
        char buffer[buffer_size];

        buffer_index += snprintf(buffer + buffer_index, buffer_size - buffer_index, "Min %.0fms\nMax %.0fms\n", min, max);

        if (!is_zext(textData, buffer)) {
            set_zext(textData, buffer);
            zextDirty->value = 1;
        }
    }
} zox_sys_end(PlotLabelSystem);
