// UserDataLink, RenderDisabled

zox_sys2(SkillOverlaySystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(UserDataLink);
    zox_sys_in(Children);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(UserDataLink, link);
        zox_sys_i(Children, children);

        if (!children->length) {
            continue;
        }

        const entity overlay = children->value[0];
        const entity data = link->value;
        if (!zox_valid(data)) {
            zox_geter_value(overlay, Scale1D, float, old_scale);
            if (old_scale) {
                zox_set(overlay, Scale1D, { 0 });
            }
            continue;
        }

        byte active = zox_has(data, SkillActive) ? zox_gett_value(data, SkillActive) : 0;

        const byte old_render_disabled = zox_gett_value(overlay, RenderDisabled);

        const byte warmup_state = zox_has(data, WarmupState) ? zox_gett_value(data, WarmupState) : 0;
        const double warmup_at = zox_has(data, WarmupAt) ? zox_gett_value(data, WarmupAt) : 0;
        const double warmup_time = zox_has(data, WarmupTime) ? zox_gett_value(data, WarmupTime) : 0;
        const byte is_warmup = warmup_state || warmup_at;

        const byte cooldown_state = zox_has(data, CooldownState) ? zox_gett_value(data, CooldownState) : 0;
        const double cooldown_at = zox_has(data, CooldownAt) ? zox_gett_value(data, CooldownAt) : 0;
        const double cooldown_time = zox_has(data, CooldownTime) ? zox_gett_value(data, CooldownTime) : 0;
        const byte is_cooldown = cooldown_state || cooldown_at;


        const byte new_render_disabled = !active && !is_warmup && !is_cooldown;
        if (old_render_disabled != new_render_disabled) {
            zox_set(overlay, RenderDisabled, { new_render_disabled });
        }

        // calculate scale
        float warmup_scale = warmup_at ? (zox_current_time - warmup_at) / warmup_time : (warmup_state ? 1 : 0);
        float cooldown_scale = cooldown_at ? (zox_current_time - cooldown_at) / cooldown_time : 0;
        float scale = active ? 1 : warmup_scale ? warmup_scale : (cooldown_scale ? 1 - cooldown_scale : 0);
        zox_set(overlay, Scale1D, { scale * 1.2f });

        // zox_log("[%f] warmup_at: %f cooldown_at [%f] - scale %f - new_render_disabled %i", zox_current_time, warmup_at, cooldown_at, scale, new_render_disabled);

    }
} zox_sys_end(SkillOverlaySystem);