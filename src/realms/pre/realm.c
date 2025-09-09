entity spawn_prefab_realm(ecs* world) {
    zox_prefab();
    zox_prefab_name("realm");
    zox_add_tag(e, Realm);
    zox_prefab_set(e, Seed, { 666 });
    zox_prefab_set(e, GenerateRealm, { zox_generate_realm_none }); // generate on start game now
    return e;
}

entity spawn_realm(ecs* world, const entity prefab) {
    if (!zox_valid(prefab)) {
        zox_log("! prefab_realm is null.");
        return 0;
    }
    zox_instance(prefab);
    zox_name("realm");
    local_realm = e;
    return e;
}
