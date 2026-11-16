// A world for tests that need no player: a realm of its own, generated on the main menu, then a terrain in it
//  - nothing streams until the test spawns a streamer (spawn_streamer, StreamerLevel 1) or a camera with one
//  - its seed: zox_tst_seed, else 666
//  - register the test with zox_tst_set_setup(name, NULL, 1) (it runs on the menus, no game started)
//  - tst sea_level, two_streamers
int zox_tst_world_frames = 0;
entity zox_tst_world_realm = 0;
entity zox_tst_world_terrain = 0;

// the terrain once its realm has generated, 0 until then
static inline entity zox_tst_world_ready(ecs* world) {
    zox_tst_world_frames++;
    if (zox_valid(zox_tst_world_terrain)) {
        return zox_tst_world_terrain;
    }
    zox_tst_waiting = "a realm of its own to generate";
    if (!zox_tst_world_realm) {
        if (!zox_valid(local_game) || !zox_valid(main_app) || zox_tst_world_frames < 5) {
            return 0;
        }
        const char* seed_text = getenv("zox_tst_seed");
        const lint seed = seed_text && seed_text[0] ? atol(seed_text) : 666;
        zox_tst_world_realm = spawn_realm(world, local_game, seed);
        zox_setv(zox_tst_world_realm, GenerateRealm, zox_generate_realm_start);
        return 0;
    }
    if (zox_getv(zox_tst_world_realm, GenerateRealm)) {
        return 0;
    }
    zox_tst_world_terrain = spawn_terrain(world, zox_tst_world_realm, zox_getv(zox_tst_world_realm, Seed));
    return 0;
}
