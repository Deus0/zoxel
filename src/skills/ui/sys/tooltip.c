// a skill's tooltip: its name, its type (instant, or cast and its time) and its numbers as they're used: damage, swing
//  (attack speed), cooldown
void skill_tooltip_text(ecs* world, entity e, char* text, int size) {
    const char* name = zox_has(e, ZoxName) ? zox_getv(e, ZoxName) : zox_get_name(e);
    int length = snprintf(text, size, "[%s] lvl [1]\n", name);
    if (zox_has(e, SkillInstant)) {
        length += snprintf(text + length, size - length, "instant");
    } else if (zox_has(e, WarmupTime)) {
        length += snprintf(text + length, size - length, "cast [%.2fs]", zox_getv(e, WarmupTime));
    }
    if (zox_has(e, CooldownTime)) {
        length += snprintf(text + length, size - length, " cooldown [%.2fs]", zox_getv(e, CooldownTime));
    }
    length += snprintf(text + length, size - length, "\n");
    if (zox_has(e, SkillDamage)) {
        const float low = zox_getv(e, SkillDamage);
        const float high = zox_has(e, SkillDamageMax) ? zox_getv(e, SkillDamageMax) : low;
        length += snprintf(text + length, size - length, high > low ? "damage [%.0f-%.0f]" : "damage [%.0f]", low, high);
    }
    if (zox_has(e, SwingTime)) {
        length += snprintf(text + length, size - length, " swing [%.2fs]", zox_getv(e, SwingTime));
    }
}

// Tooltip Event for skill icons
void skill_icon_tooltip_system(iter* it) {
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
        if (!zox_valid(e2) || !zox_has(e2, Skill)) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
        if (!zox_valid(tooltip)) {
            zox_loge("Tooltip not found in canvas");
            continue;
        }
        char result[256];
        skill_tooltip_text(world, e2, result, sizeof(result));
        set_tooltip_text(world, e, tooltip, result);
        // zox_log("Skill Tooltip [%s]", result);
    }
    zox_sys_on_end();
} zoxd_system(skill_icon_tooltip_system);
