// NOTE: A melee skill's warmup starts its user's swing, so the wind up plays before the hit, not after it
//  - the swing runs over warmup + cooldown from the warmup's start: the strike lands as the warmup ends (melee.c hits)
//  - set once a warmup (SwingStart is its WarmupAt), out of energy it doesn't swing (melee.c plays no resource)
void melee_swing_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(WarmupAt);
    zox_sys_in(WarmupTime);
    zox_sys_in(CooldownTime);
    zox_sys_in(SkillCost);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(WarmupAt, at);
        zox_sys_i(WarmupTime, warmup);
        zox_sys_i(CooldownTime, cooldown);
        zox_sys_i(SkillCost, cost);
        if (!at->value) {
            continue;
        }
        const entity user = zox_get_parent(world, e);
        const entity skeleton = zox_valid(user) ? get_skeleton(world, user) : 0;
        if (!zox_valid(skeleton) || zox_getv(skeleton, SwingStart) == at->value) {
            continue;
        }
        if (!disable_skill_costs) {
            const entity resource = get_user_stat_from_realm(world, user, zox_get_link(world, e, SkillResource));
            if (!zox_valid(resource) || !zox_has(resource, StatValue) || zox_getv(resource, StatValue) < cost->value) {
                continue;
            }
        }
        zox_setv(skeleton, SwingStart, at->value);
        zox_setv(skeleton, SwingSpeed, warmup->value + cooldown->value);
    }
    zox_sys_on_end();
} zoxd_system(melee_swing_system);
