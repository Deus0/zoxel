// Tooltip Event for item icons
// NOTE: Quests have no slots, just direct data linked to ui
void stat_icon_label_event(iter* it) {
    byte dbg_log = 0;
    byte label_text_capacity = 8;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(TextData, text);
        entity stat = zox_has(e, DataLink) ?
            zox_getv(e, DataLink) :
            0;
        if (!zox_valid(stat) ||
            !zox_has(stat, Stat))
        {
            continue;
        }
        zox_geter(stat, StatValue, value);
        int val = floor(value->value);
        char result[label_text_capacity];
        if (zox_has(stat, StatState)) {
            zox_geter(stat, StatValueMax, maxx);
            int max = ceil(maxx->value);
            snprintf(result, label_text_capacity, "%i", max);
        } else if (!val) {
            result[0] = '\0';
        } else if (zox_has(stat, StatLevel)) {
            snprintf(result, label_text_capacity, "%i", val); // Lvl
        } else {
            snprintf(result, label_text_capacity, "%i", val);
        }
        // set text of quantity label
        if (!is_zext(text, result)) {
            set_zext(text, result);
            zox_add(e, Dirty);
            if (dbg_log) {
                zox_log("Stat Frame Label [%s] Set to [%s]",
                    zox_getn(e),
                    result);
            }
        }
    }
}

// Tooltip Event for item icons
void stat_icon_tooltip_event(iter* it) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DataLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DataLink, data);
        /*if (state->value != zox_state_select_active) {
            continue;
        }*/
        // zox_log("quest_icon_tooltip_event [%s]", zox_sys_e_name);
        entity stat = data->value;
        // TODO: We should do this for only X types of items
        //      - BlockItem, ConsumeableItem, etc
        if (!zox_valid(stat) ||
            !zox_has(stat, Stat))
        {
            continue;
        }
        entity canvas = zox_get_parent_by_id(
            world,
            e,
            zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        entity tooltip = zox_get_child_by_id(
            world,
            canvas,
            zox_id(Tooltip));
        if (!tooltip) {
            zox_loge("Tooltip not found in canvas");
            continue;
        }
        char result[128];
        const char *name = zox_has(stat, ZoxName) ?
            zox_getv(stat, ZoxName) : "none";
        if (zox_has(stat, StatLevel)) {
            float value = zox_getv(stat, StatValue);
            float experience = zox_getv(stat, ExperienceValue);
            float experience_max = zox_getv(stat, ExperienceMax);
            sprintf(result, "[%s] lvl %i [%i/%i]\n", name, (int) value, (int) experience, (int) experience_max);
        } else if (zox_has(stat, StatState)) {
            float value = zox_getv(stat, StatValue);
            float value_max = zox_getv(stat, StatValueMax);
            sprintf(result, "[%s] [%i/%i]\n", name, (int) value, (int) value_max);
        } else if (zox_has(stat, StatRegen)) {
            float value = zox_getv(stat, StatValue);
            sprintf(result, "[%s] [%i]\n", name, (int) value);
        } else if (zox_has(stat, StatAttribute)) {
            float value = zox_getv(stat, StatValue);
            sprintf(result, "[%s] [%i]\n", name, (int) value);
        } else {
            sprintf(result, "[%s]\n", name);
        }
        set_tooltip_text(world, e, tooltip, result);
    }
}
