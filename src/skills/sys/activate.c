void SkillActivateSystem(ecs_iter_t *it) {
    zox_sys_begin();
    zox_sys_in(Activate);
    zox_sys_out(SkillActive);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Activate, activate);
        zox_sys_o(SkillActive, skill_active);
        if (activate->value == zox_dirty_active) {
            skill_active->value = 1;
        }
    }
} zoxd_system2(SkillActivateSystem);

void SkillWarmupSystem(ecs_iter_t *it) {
    zox_sys_begin();
    zox_sys_in(WarmupState);
    zox_sys_out(SkillActive);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(WarmupState, state);
        zox_sys_o(SkillActive, skill_active);
        if (state->value == zox_dirty_active) {
            skill_active->value = 1;
            zox_log("SkillWarmupSystem %i", state->value);
        }
    }
} zoxd_system2(SkillWarmupSystem);