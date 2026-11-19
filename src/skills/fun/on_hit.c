// a realm's debuffs (burn, poison): prefabs (variants of prefab_burn, prefab_poison) its children, with their own
//  Damage, Rate (a second), DamageTime (0 lasts) and Color; skills link the ones they put on who they hit (OnHitLinks:
//  shoot's burn) or touch (an aura's poison)
entity spawn_realm_debuff(ecs* world, entity realm, entity prefab, const char* name, float damage, float rate, double time, color c) {
    const entity e = zox_prefab_variant(world, prefab);
    zox_set_unique_name(e, name);
    zox_setv(e, Damage, damage);
    zox_setv(e, Rate, rate);
    zox_setv(e, DamageTime, time);
    zox_setv(e, Color, c);
    zox_set_parent(world, e, realm);
    return e;
}

// an instance of a debuff on who it's put, from a skill's user, for its DamageTime
entity spawn_debuff(ecs* world, entity prefab, entity target, entity user, entity skill) {
    zox_instance(prefab);
    zox_name("debuff");
    zox_setv(e, DestroyInTime, zox_getv(prefab, DamageTime));
    zox_set_parent(world, e, target);
    zox_link(world, e, Spawner, user);
    zox_link(world, e, Skill, skill);
    return e;
}

// a skill's hit puts its debuffs on who it hit; hit again, the same one starts over
void skill_on_hit(ecs* world, entity skill, entity user, entity target) {
    if (!zox_valid(skill) || !zox_valid(target) || zox_has(target, Dead)) {
        return;
    }
    entity debuffs[32];
    const uint count = zox_get_children_by_id(world, target, debuffs, 32, zox_id(Dot));
    zox_each_link(world, skill, OnHitLinks, prefab) {
        byte again = 0;
        for (uint i = 0; i < count && !again; i++) {
            if (zox_is_instance_of(debuffs[i], prefab) && zox_get_link(world, debuffs[i], Skill) == skill) {
                zox_setv(debuffs[i], DestroyInTime, zox_getv(prefab, DamageTime));
                again = 1;
            }
        }
        if (!again) {
            spawn_debuff(world, prefab, target, user, skill);
        }
    }
}
