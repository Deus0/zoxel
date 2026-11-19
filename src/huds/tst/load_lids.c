// tst: load_lids, loading closes the player's eyes and opens them once playing (load_lids_system, CameraLids, 2D)
//  - from boot into a new game: its camera's lids watched each frame
//  - while loading (load_dark) they reach shut (1), the "Loading" ui up; once playing and the player spawned, they
//    open (0) and loading lets go of them (load_lids 0)
//  - run: bash bsh/tst.sh load_lids
float zox_tst_lload_max = 0;
byte zox_tst_lload_shut_loading = 0;
byte zox_tst_lload_ui_shut = 0;
double zox_tst_lload_spawned = 0;

byte zox_tst_load_lids_ready(ecs* world) {
    const entity player = zox_players[0];
    const entity camera = zox_valid(player) ? zox_get_link(world, player, CameraLink) : 0;
    if (zox_valid(camera) && zox_has(camera, CameraLids)) {
        const float lids = zox_getv(camera, CameraLids);
        zox_tst_lload_max = float_max(zox_tst_lload_max, lids);
        zox_tst_lload_shut_loading |= load_dark && lids >= 1;
        const entity canvas = zox_get_link(world, player, CanvasLink);
        zox_tst_lload_ui_shut |= lids >= 1 && zox_valid(canvas) && zox_valid(zox_get_child_by_id(world, canvas, zox_id(LoadUI)));
    }
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    zox_tst_waiting = "the eyes to open playing";
    if (!zox_tst_lload_spawned) {
        zox_tst_lload_spawned = zox_current_time;
    }
    zox_tst_progress(zox_valid(camera) ? zox_getv(camera, CameraLids) : -1);
    return zox_valid(camera) && !load_lids && zox_getv(camera, CameraLids) == 0;
}

void zox_tst_load_lids_check(ecs* world) {
    const entity player = zox_players[0];
    const entity camera = zox_valid(player) ? zox_get_link(world, player, CameraLink) : 0;
    const float lids = zox_valid(camera) && zox_has(camera, CameraLids) ? zox_getv(camera, CameraLids) : -1;
    zox_log("[tst] load_lids: most shut [%.2f], shut loading [%i], with the load ui [%i], now [%.2f]", zox_tst_lload_max,
        zox_tst_lload_shut_loading, zox_tst_lload_ui_shut, lids);
    zox_tst_check(zox_tst_lload_shut_loading, "the eyes never shut loading (most [%.2f])", zox_tst_lload_max);
    zox_tst_check(zox_tst_lload_ui_shut, "the load ui was never up with the eyes shut");
    zox_tst_check(lids == 0 && !load_lids && !load_dark, "playing, the eyes aren't open [%.2f] (loading has them [%i])", lids, load_lids);
}

void zox_tsts_huds_load_lids() {
    zox_tst_add("load_lids", zox_tst_load_lids_ready, zox_tst_load_lids_check, 120);
}
