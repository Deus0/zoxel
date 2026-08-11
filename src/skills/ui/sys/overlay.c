// Called from the skill icons
// TODO: Make generic for any entity ui with cooldown of data (items with cooldowns)
zox_sys2(SkillOverlaySystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DataLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DataLink, link);
        entity overlay = zox_get_child_by_id(world, e, zox_id(IconOverlay));
        if (!zox_valid(overlay)) {
            continue;
        }
        if (!zox_has(overlay, LocalScale1)) {
            zox_loge("Icon Overlay [%s] has no LocalScale1", zox_getn(overlay));
            continue;
        }
        byte has_scale2 = zox_has(overlay, LocalScale2);
        entity data = link->value;
        if (!zox_valid(data)) {
            if (!has_scale2) {
                float old_scale = zox_getv(overlay, LocalScale1);
                if (old_scale) {
                    zox_setv(overlay, LocalScale1, 0);
                }
            } else {
                float old_scale = zox_getv(overlay, LocalScale2).x;
                if (old_scale) {
                    zox_setv(overlay, LocalScale2, float2_zero);
                }
            }
            continue;
        }
        byte active = zox_has(data, SkillActive) ? zox_getv(data, SkillActive) : 0;
        byte old_render_disabled = zox_getv(overlay, RenderDisabled);
        byte warmup_state = zox_has(data, WarmupState) ? zox_getv(data, WarmupState) : 0;
        double warmup_at = zox_has(data, WarmupAt) ? zox_getv(data, WarmupAt) : 0;
        double warmup_time = zox_has(data, WarmupTime) ? zox_getv(data, WarmupTime) : 0;
        byte is_warmup = warmup_state || warmup_at;
        byte cooldown_state = zox_has(data, CooldownState) ? zox_getv(data, CooldownState) : 0;
        double cooldown_at = zox_has(data, CooldownAt) ? zox_getv(data, CooldownAt) : 0;
        double cooldown_time = zox_has(data, CooldownTime) ? zox_getv(data, CooldownTime) : 0;
        byte is_cooldown = cooldown_state || cooldown_at;
        byte new_render_disabled = !active && !is_warmup && !is_cooldown;
        if (old_render_disabled != new_render_disabled) {
            zox_setv(overlay, RenderDisabled, new_render_disabled);
        }
        // calculate scale
        float warmup_scale = warmup_at ? (zox_current_time - warmup_at) / warmup_time : (warmup_state ? 1 : 0);
        float cooldown_scale = cooldown_at ? (zox_current_time - cooldown_at) / cooldown_time : 0;
        float scale = active ? 1 : warmup_scale ? warmup_scale : (cooldown_scale ? 1 - cooldown_scale : 0);
        if (!has_scale2) {
            zox_setm(overlay, LocalScale1, scale * 1.2f);
        } else {
            // we should use a setm_raw here instead
            zox_setv(overlay, LocalScale2, float2_single(scale * 1.2f));
        }
        if (dbg_log && scale) {
            zox_log("Overlay Skill System [%s] -> [%s] - scale2 [%i]", zox_getn(data), zox_getn(overlay), has_scale2);
            zox_log("   - at [%f],  warmup_at: %f, cooldown_at [%f]", zox_current_time, warmup_at, cooldown_at);
            zox_log("   - scale %f - new_render_disabled %i",   scale, new_render_disabled);
        }
    }
} zox_sys_end(SkillOverlaySystem);
