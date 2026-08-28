entity spawn_prefab_realm(ecs* world) {
    zox_prefab();
    zox_prefab_name("realm");
    zox_add(e, Realm);
    zox_setv(e, Seed, 666);
    zox_setv(e, GenerateRealm, zox_generate_realm_none);
    return e;
}

entity spawn_realm(
    ecs* world,
    entity game,
    lint seed)
{
    zox_instance(prefab_realm);
    zox_name("realm");
    zox_setv(e, Seed, seed);
    zox_set_parent(world, e, game);
    zox_setv(game, RealmLink, e);
    local_realm = e;
    set_noise_seed(seed);
    return e;
}
