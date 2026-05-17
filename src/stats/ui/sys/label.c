// NOTE: Sets the stat icon labels
zox_sys2(StatIconLabelSystem) {
    byte label_text_capacity = 64;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DataLink);
    zox_sys_out(TextData);
    zox_sys_out(TextDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(DataLink, data);
        zox_sys_o(TextData, text);
        zox_sys_o(TextDirty, dirty);
        entity e2 = data->value;
        if (!zox_valid(e2) || !zox_has(e2, Stat) || !zox_has(e2, StatDirty) || !zox_has(e2, StatValue)) {
            continue;
        }
        zox_geter_value(e2, StatDirty, byte, stat_dirty);
        if (text->length && stat_dirty != zox_dirty_active) {
            continue;
        }
        zox_geter(e2, StatValue, value);
        int val = floor(value->value);
        char result[label_text_capacity];
        if (zox_has(e2, StatState)) {
            zox_geter(e2, StatValueMax, maxx);
            int max = ceil(maxx->value);
            snprintf(result, label_text_capacity, "%i", max);
        } else if (zox_has(e2, StatLevel)) {
            snprintf(result, label_text_capacity, "%i", val); // Lvl
        } else {
            snprintf(result, label_text_capacity, "%i", val);
        }
        // set text of quantity label
        if (!is_zext(text, result)) {
            set_zext(text, result);
            dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(StatIconLabelSystem);

/*zox_sys2(StatIconLabelSystem) {
    int label_text_count = 64;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StatLink);
    zox_sys_out(TextData);
    zox_sys_out(TextDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(StatLink, stat);
        zox_sys_o(TextData, data);
        zox_sys_o(TextDirty, dirty);

        if (!zox_valid(e2)) {
            continue;
        }

        zox_geter_value(e2, StatDirty, byte, stat_dirty);

        if (data->length && stat_dirty != zox_dirty_active) {
            continue;
        }

        zox_geter(e2, StatValue, statValue);
        int value = floor(statValue->value);

        // set text of stat label
        if (!is_zext(data, text)) {
            set_zext(data, text);
            dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(StatIconLabelSystem);*/
