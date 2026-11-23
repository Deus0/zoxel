// A world for tests that need no player: a realm of its own, generated on the main menu, then a terrain in it
//  - nothing streams until the test spawns a streamer (spawn_streamer, StreamerLevel 1) or a camera with one
//  - its seed: zox_tst_seed, else 666
//  - register the test with zox_tst_set_setup(name, NULL, 1) (it runs on the menus, no game started), or with
//    zox_tst_world_setup for no player at all: no menu, canvas or camera, the test spawns the camera it wants
//    (spawn_viewport_camera; prefab_add_streamer to stream and cull chunks round it, as a player's does)
//  - tst sea_level, two_streamers
int zox_tst_world_frames = 0;
entity zox_tst_world_realm = 0;
entity zox_tst_world_terrain = 0;

// no player at boot: nothing but the app, its game and what the test spawns (lod_flyby)
void zox_tst_world_setup() {
    zox_is_players = 0;
}

// the terrain once its realm has generated, 0 until then
entity zox_tst_world_ready(ecs* world) {
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
        // a save folder of its own, in the run's (--nosaves: thrown away): without one its chunks saved beside the
        //  binary and the next run loaded them
        char home_path[max_path_characters];
        get_home_directory(home_path, sizeof(home_path));
        char* game_path = join_paths(home_path, game_name);
        create_new_directory(game_path);
        char* save_dir = join_paths(game_path, "tst_world");
        create_new_directory(save_dir);
        set_FolderPath(world, zox_tst_world_realm, save_dir);
        free(save_dir);
        free(game_path);
        zox_setv(zox_tst_world_realm, GenerateRealm, zox_generate_realm_start);
        return 0;
    }
    if (zox_getv(zox_tst_world_realm, GenerateRealm)) {
        return 0;
    }
    zox_tst_world_terrain = spawn_terrain(world, zox_tst_world_realm, zox_getv(zox_tst_world_realm, Seed));
    return 0;
}
