entity spawn_prefab_realm(ecs* world) {
    zox_prefab();
    zox_prefab_name("realm");
    zox_add(e, Realm);
    zox_setv(e, Seed, 666);
    zox_setv(e, GenerateRealm, zox_generate_realm_none);
    zox_set(e, FolderPath, { 0 });
    // its own time: its terrain's and all in it follow it, paused it stops (timing/sys/clock.c)
    zox_setv(e, ClockTime, 0);
    zox_setv(e, ClockDelta, 0);
    zox_setv(e, ClockScale, 1);
    return e;
}

extern void zox_link_game(ecs*, entity, entity);

entity spawn_realm(
    ecs* world,
    entity game,
    lint seed)
{
    zox_instance(prefab_realm);
    zox_name("realm");
    zox_setv(e, Seed, seed);
#ifdef zox_tests
    // a test on a random world that fails runs again on it: zox_tst_seed=<seed> bash bsh/tst.sh <name>
    zox_log("[tst] world seed [%lld]", (long long) seed);
#endif
    // global math function, bad
    set_noise_seed(seed);
    // zox_set_parent(world, e, game);
    zox_link(world, game, RealmLink, e);
    // zox_link(world, e, GameLink, game);
    zox_link_game(world, e, game);
    local_realm = e;
    return e;
}
