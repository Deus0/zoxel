entity spawn_prefab_realm(ecs* world) {
    zox_prefab();
    zox_prefab_name("realm");
    zox_add(e, Realm);
    zox_setv(e, Seed, 666);
    zox_setv(e, GenerateRealm, zox_generate_realm_none);
    return e;
}

entity spawn_realm(ecs* world, entity prefab, entity game) {
    zox_instance(prefab);
    zox_name("realm");
    local_realm = e;
    zox_set_parent(world, e, game);
    zox_setv(game, RealmLink, e);
    return e;
}
