zox_sys2(StatTextSystem) {
    int label_text_count = 256;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(StatLink);
    zox_sys_out(TextData);
    zox_sys_out(TextDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(StatLink, stat_link);
        zox_sys_o(TextData, data);
        zox_sys_o(TextDirty, dirty);
        entity stat = stat_link->value;
        if (!zox_valid(stat)) {
            zox_sys_e();
            zox_log_error("[%s] has invalid stat linked", zox_get_name(e));
            continue;
        }
        if (!zox_has(stat, ZoxName) || !zox_has(stat, StatValue)) {
            zox_sys_e();
            zox_log_error("[%s] Stat [%s] has Invalid Components", zox_get_name(e), zox_get_name(stat));
            continue;
        }
        if (!zox_valid(stat)) {
            continue;
        }
        zox_geter(stat, StatValue, value);
        zox_geter(stat, ZoxName, stat_name);
        int value_floored = floor(value->value);
        char text[label_text_count];
        if (zox_has(stat, StatState)) {
            zox_geter(stat, StatValueMax, max)
            int max_value = ceil(max->value);
            snprintf(text, label_text_count, "%s [%i/%i]", stat_name->value, value_floored, max_value);
        } else if (zox_has(stat, StatLevel)) {
            zox_geter(stat, ExperienceValue, experience)
            zox_geter(stat, ExperienceMax, experience_max)
            int experience_i = ceil(experience->value);
            int experience_max_i = ceil(experience_max->value);
            snprintf(text, label_text_count, "%s Lvl %i [%i/%i]", stat_name->value, value_floored, experience_i, experience_max_i);
        } else {
            snprintf(text, label_text_count, "%s [%i]", stat_name->value, value_floored);
        }
        if (!is_zext(data, text)) {
            set_zext(data, text);
            dirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(StatTextSystem);
