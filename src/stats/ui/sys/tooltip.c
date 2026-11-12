// Tooltip Event for item icons
/*void stat_icon_tooltip_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SelectState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SelectState, state);
        entity data = zox_get_link(world, e, DataLink);
        if (state->value != zox_state_select_active) {
            continue;
        }
        entity e2 = data;
        if (!zox_valid(e2) || !zox_has(e2, Stat)) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
        if (!tooltip) {
            zox_loge("Tooltip not found in canvas");
            continue;
        }
        char result[128];
        const char *name = zox_has(e2, ZoxName) ? zox_getv(e2, ZoxName) : "none";
        if (zox_has(e2, StatLevel)) {
            float value = zox_getv(e2, StatValue);
            float experience = zox_getv(e2, ExperienceValue);
            float experience_max = zox_getv(e2, ExperienceMax);
            sprintf(result, "[%s] lvl %i [%i/%i]\n", name, (int) value, (int) experience, (int) experience_max);
        } else if (zox_has(e2, StatState)) {
            float value = zox_getv(e2, StatValue);
            float value_max = zox_getv(e2, StatValueMax);
            sprintf(result, "[%s] [%i/%i]\n", name, (int) value, (int) value_max);
        } else if (zox_has(e2, StatRegen)) {
            float value = zox_getv(e2, StatValue);
            sprintf(result, "[%s] [%i]\n", name, (int) value);
        } else if (zox_has(e2, StatAttribute)) {
            float value = zox_getv(e2, StatValue);
            sprintf(result, "[%s] [%i]\n", name, (int) value);
        } else {
            sprintf(result, "[%s]\n", name);
        }
        set_tooltip_text(world, e, tooltip, result);
    }
    zox_sys_on_end();
} zoxd_system(stat_icon_tooltip_system);*/

