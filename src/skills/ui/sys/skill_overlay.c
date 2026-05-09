// Called from the skill icons
zox_sys2(SkillOverlaySystem) {
    byte debug_system = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(UserDataLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(UserDataLink, link);
        entity children[layouts2_children_capacity];
        uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
        if (!children_length) {
            continue;
        }
        entity overlay = children[0];
        if (!zox_valid(overlay)) {
            continue;
        }
        entity data = link->value;
        if (!zox_valid(data)) {
            zox_geter_value(overlay, Scale1D, float, old_scale);
            if (old_scale) {
                zox_set(overlay, Scale1D, { 0 });
            }
            continue;
        }
        byte active = zox_has(data, SkillActive) ? zox_gett_value(data, SkillActive) : 0;
        byte old_render_disabled = zox_gett_value(overlay, RenderDisabled);
        byte warmup_state = zox_has(data, WarmupState) ? zox_gett_value(data, WarmupState) : 0;
        double warmup_at = zox_has(data, WarmupAt) ? zox_gett_value(data, WarmupAt) : 0;
        double warmup_time = zox_has(data, WarmupTime) ? zox_gett_value(data, WarmupTime) : 0;
        byte is_warmup = warmup_state || warmup_at;
        byte cooldown_state = zox_has(data, CooldownState) ? zox_gett_value(data, CooldownState) : 0;
        double cooldown_at = zox_has(data, CooldownAt) ? zox_gett_value(data, CooldownAt) : 0;
        double cooldown_time = zox_has(data, CooldownTime) ? zox_gett_value(data, CooldownTime) : 0;
        byte is_cooldown = cooldown_state || cooldown_at;
        byte new_render_disabled = !active && !is_warmup && !is_cooldown;
        if (old_render_disabled != new_render_disabled) {
            zox_set(overlay, RenderDisabled, { new_render_disabled });
        }
        // calculate scale
        float warmup_scale = warmup_at ? (zox_current_time - warmup_at) / warmup_time : (warmup_state ? 1 : 0);
        float cooldown_scale = cooldown_at ? (zox_current_time - cooldown_at) / cooldown_time : 0;
        float scale = active ? 1 : warmup_scale ? warmup_scale : (cooldown_scale ? 1 - cooldown_scale : 0);
        zox_set(overlay, Scale1D, { scale * 1.2f });
        if (debug_system) {
            zox_log("Overlay Skill System [%s] -> [%s]", zox_get_name(data), zox_get_name(overlay));
            zox_log("   - at [%f],  warmup_at: %f, cooldown_at [%f]", zox_current_time, warmup_at, cooldown_at);
            zox_log("   - scale %f - new_render_disabled %i",   scale, new_render_disabled);
        }
    }
} zox_sys_end(SkillOverlaySystem);
