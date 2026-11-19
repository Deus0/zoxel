// burn: a slow damage over time (a Dot) on a character a skill with Burn hits (shoot's), its skill's colour
//  rising off them (a particle streamer, its child), gone in its time (DestroyInTime); hit again, the same skill's burn starts over
entity spawn_prefab_burn(ecs *world) {
    zox_prefab();
    zox_prefab_name("burn");
    zox_add(e, Dot);
    zox_setv(e, SkillDamage, 0);
    zox_setv(e, DestroyInTime, 0);
    // placed on who it's on (its parent), so its flames (a streamer, its child) follow them
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, quaternion_identity);
    zox_setv(e, Color, color_white);
    add_prefab_particle3_streamer(world, e, 24, 0.6f, float3_single(0.3f), 3);
    return e;
}

entity burn_character(ecs* world, entity target, entity spawner, entity skill) {
    if (!zox_valid(skill) || !zox_has(skill, Burn) || !zox_valid(target) || zox_has(target, Dead)) {
        return 0;
    }
    const float2 burn = zox_getv(skill, Burn);     // damage a second, seconds
    entity debuffs[32];
    const uint count = zox_get_children_by_id(world, target, debuffs, 32, zox_id(Dot));
    for (uint i = 0; i < count; i++) {
        if (zox_get_link(world, debuffs[i], Skill) == skill) {     // its burn (a dot of its skill)
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
    zox_set(e, Color, { zox_has(skill, Color) ? zox_getv(skill, Color) : color_white });   // its flames' (its streamer's)
    return e;
}
