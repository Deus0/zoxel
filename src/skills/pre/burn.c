// burn: a short damage over time (a Dot) on a character a skill with SkillBurn hits (shoot's), its skill's colour
//  rising off them (a particle stream), gone in its time (DestroyInTime); hit again, the same skill's burn starts over
entity spawn_prefab_burn(ecs *world) {
    zox_prefab();
    zox_prefab_name("burn");
    zox_add(e, Dot);
    zox_add(e, Burn);
    zox_setv(e, SkillDamage, 0);
    zox_setv(e, DestroyInTime, 0);
    return e;
}

entity burn_character(ecs* world, entity target, entity spawner, entity skill) {
    if (!zox_valid(skill) || !zox_has(skill, SkillBurn) || !zox_valid(target) || zox_has(target, Dead)) {
        return 0;
    }
    const float2 burn = zox_getv(skill, SkillBurn);     // damage a second, seconds
    entity debuffs[32];
    const uint count = zox_get_children_by_id(world, target, debuffs, 32, zox_id(Burn));
    for (uint i = 0; i < count; i++) {
        if (zox_get_link(world, debuffs[i], Skill) == skill) {
            zox_setv(debuffs[i], DestroyInTime, burn.y);
            return debuffs[i];
        }
    }
    zox_instance(prefab_burn);
    zox_name("burn");
    zox_setv(e, SkillDamage, -burn.x);
    zox_setv(e, DestroyInTime, burn.y);
    zox_set_parent(world, e, target);
    zox_link(world, e, Spawner, spawner);
    zox_link(world, e, Skill, skill);
    const float3 bounds = zox_has(target, Bounds3D) ? zox_getv(target, Bounds3D) : float3_single(0.5f);
    add_particle3_stream(world, e, 24, 0.6f, float3_scale(bounds, 0.6f), 1.2f,
        zox_has(skill, Color) ? zox_getv(skill, Color) : color_white);
    return e;
}
