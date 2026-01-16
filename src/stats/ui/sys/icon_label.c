zox_sys2(StatIconLabelSystem) {
    int label_text_count = 64;
    //char stat_name_text[stat_name_text_count];

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StatLink);
    zox_sys_out(TextData);
    zox_sys_out(TextDirty);

    for (int i = 0; i < it->count; i++) {

        // zox_sys_e();
        zox_sys_i(StatLink, stat);
        zox_sys_o(TextData, data);
        zox_sys_o(TextDirty, dirty);

        if (!zox_valid(stat->value)) {
            continue;
        }

        zox_geter_value(stat->value, StatDirty, byte, stat_dirty);

        if (data->length && stat_dirty != zox_dirty_active) {
            continue;
        }

        zox_geter(stat->value, StatValue, statValue);
        int value = floor(statValue->value);

        char text[label_text_count];
        if (zox_has(stat->value, StatState)) {
            zox_geter(stat->value, StatValueMax, statValueMax)
            int max = ceil(statValueMax->value);
            snprintf(text, label_text_count, "%i/%i", value, max);
        } else if (zox_has(stat->value, StatLevel)) {
            snprintf(text, label_text_count, "%i", value); // Lvl
        } else {
            snprintf(text, label_text_count, "%i", value);
        }

        // set text of stat label
        if (!is_zext(data, text)) {
            set_zext(data, text);
            dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(StatIconLabelSystem);