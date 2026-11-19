// burn: a dot (Dot, Burn), a realm's (spawn_realm_debuff) that skills put on who they hit (OnHitLinks, fun/on_hit.c):
//  its Damage times its Rate a second for its DamageTime, its colour rising off them (a particle streamer, its child)
entity spawn_prefab_burn(ecs *world) {
    zox_prefab();
    zox_prefab_name("burn");
    zox_add(e, Dot);
    zox_add(e, Burn);
    zox_setv(e, Damage, 0);
    zox_setv(e, Rate, 1);
    zox_setv(e, DamageTime, 0);
    zox_setv(e, DestroyInTime, 0);
    // placed on who it's on (its parent), so its flames (a streamer, its child) follow them
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, quaternion_identity);
    zox_setv(e, Color, color_white);
    add_prefab_particle3_streamer(world, e, 24, 0.6f, float3_single(0.3f), 3);
    return e;
}
