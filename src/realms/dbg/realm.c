entity zox_dbg_realm;

extern entity local_game;

entity test_spawn_realm(ecs* world) {
    if (zox_dbg_realm) {
        zox_delete(zox_dbg_realm);
        zox_dbg_realm = 0;
    }
    lint seed = 666;
    zox_dbg_realm = spawn_realm(
        world,
        local_game,
        seed);
    zox_setv(zox_dbg_realm, GenerateRealm, zox_generate_realm_start);
}