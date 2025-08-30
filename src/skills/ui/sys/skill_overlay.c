// UserDataLink, RenderDisabled

void SkillOverlaySystem(iter *it) {
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

        const entity data = link->value;
        if (!zox_valid(data)) {
            continue;
        }

        const entity overlay = children->value[0];
        byte active = zox_has(data, SkillActive) ? zox_gett_value(data, SkillActive) : 0;

        const byte old_render_disabled = zox_gett_value(overlay, RenderDisabled);

        const double warmup_at = zox_has(data, WarmupAt) ? zox_gett_value(data, WarmupAt) : 0;
        const double warmup_time = zox_has(data, WarmupTime) ? zox_gett_value(data, WarmupTime) : 0;

        const double cooldown_at = zox_has(data, CooldownAt) ? zox_gett_value(data, CooldownAt) : 0;
        const double cooldown_time = zox_has(data, CooldownTime) ? zox_gett_value(data, CooldownTime) : 0;


        const byte new_render_disabled = !active && !warmup_at && !cooldown_at;
        if (old_render_disabled != new_render_disabled) {
            zox_set(overlay, RenderDisabled, { new_render_disabled });
        }

        // calculate scale
        float warmup_scale = warmup_at ? (zox_current_time - warmup_at) / warmup_time : 0;
        float cooldown_scale = cooldown_at ? (zox_current_time - cooldown_at) / cooldown_time : 0;
        float scale = active ? 1 : warmup_scale ? warmup_scale : (cooldown_scale ? 1 - cooldown_scale : 0);
        zox_set(overlay, Scale1D, { scale });

        // zox_log("[%f] warmup_at: %f cooldown_at [%f] - scale %f - new_render_disabled %i", zox_current_time, warmup_at, cooldown_at, scale, new_render_disabled);

    }
} zoxd_system2(SkillOverlaySystem);