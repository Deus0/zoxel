// tst: xr_continue, continue a save and pause, with XR on the simulator by the controllers alone (3D huds)
//  - saves as load_games makes them (tst/load_home); A on the start menu, A on continue (the main menu's first)
//  - the load menu is a 3D list (Canvas3: the 2D one was unseen in a headset); A on its first, the newest save, goes
//      straight in (no realm menu, 2D too); playing, its realm the save's
//  - the left hand's menu button pauses: the pause menu a 3D list; A on its first (return) plays on
//  - run: bash bsh/tst.sh xr_continue
int zox_tst_xr_continue_stage = 0;
int zox_tst_xr_continue_frames = 0;
byte zox_tst_xr_continue_load3 = 0;
byte zox_tst_xr_continue_loaded = 0;
byte zox_tst_xr_continue_paused3 = 0;
byte zox_tst_xr_continue_resumed = 0;

static inline void zox_tst_xr_continue_next() {
    zox_tst_xr_continue_stage++;
    zox_tst_xr_continue_frames = 0;
}

void zox_tst_xr_continue_setup() {
    zox_tst_xr_sim_setup();
    zox_tst_load_games_setup();
}

byte zox_tst_xr_continue_ready(ecs* world) {
    const entity player = zox_players[0];
    if (!zox_valid(xr_camera) || !zox_valid(player)) {
        zox_tst_waiting = "the xr camera";
        return 0;
    }
    const entity canvas = zox_get_link(world, player, CanvasLink);
    const entity game = zox_get_link(world, player, GameLink);
    const byte game_state = zox_valid(game) ? zox_getv(game, GameState) : 0;
    const int f = ++zox_tst_xr_continue_frames;
    zox_tst_progress(zox_tst_xr_continue_stage * 1000 + f);
    // A held a few frames then let go (a click is its release), again every 60 frames until the stage moves on
    const byte presses_a = zox_tst_xr_continue_stage <= 2 || zox_tst_xr_continue_stage == 5;
    xr_sim_script_a = presses_a && f % 60 >= 20 && f % 60 < 24;
    xr_sim_script_start = zox_tst_xr_continue_stage == 4 && f >= 20 && f < 24;
    switch (zox_tst_xr_continue_stage) {
        case 0:
            zox_tst_waiting = "the start menu";
            if (f >= 30 && !zox_valid(zox_get_link(world, player, MenuStart))) {
                zox_tst_xr_continue_next();
            }
            return 0;
        case 1:
            zox_tst_waiting = "the main menu, then continue";
            {
                const entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuLoad));
                if (zox_valid(menu)) {
                    zox_tst_xr_continue_load3 = zox_has(menu, Canvas3);
                    zox_tst_xr_continue_next();
                }
            }
            return 0;
        case 2:
            zox_tst_waiting = "the load menu, then the newest save";
            if (!zox_valid(zox_get_child_by_id(world, canvas, zox_id(MenuLoad))) && f > 24) {
                zox_tst_xr_continue_next();
            }
            return 0;
        case 3:
            zox_tst_waiting = "playing the save";
            // the realm menu, 3D: Enter
            zox_tst_xr_realm_enter(world, canvas, f);
            if (game_state == zox_game_state_playing) {
                const entity realm = zox_get_link(world, game, RealmLink);
                if (zox_valid(realm) && zox_has(realm, FolderPath)) {
                    const int index = zox_tst_load_save_index(zox_get(realm, FolderPath)->value);
                    zox_log("[tst] xr_continue: playing [%s], save [%i]", zox_get(realm, FolderPath)->value, index);
                    zox_tst_xr_continue_loaded = index == zox_tst_load_saves - 1;
                }
                zox_tst_xr_continue_next();
            }
            return 0;
        case 4:
            zox_tst_waiting = "the menu button to pause";
            {
                const entity menu = get_pause_menu(world);
                if (zox_valid(menu) && f > 30) {
                    zox_tst_xr_continue_paused3 = zox_has(menu, Canvas3);
                    zox_tst_xr_continue_next();
                }
            }
            return 0;
        case 5:
            zox_tst_waiting = "A on return, playing again";
            if (game_state == zox_game_state_playing && f > 24 &&
                !zox_valid(get_pause_menu(world))) {
                zox_tst_xr_continue_resumed = 1;
                return 1;
            }
            return 0;
    }
    return 0;
}

void zox_tst_xr_continue_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_xr_continue_load3, "the load menu isn't a 3D list");
    zox_tst_check(zox_tst_xr_continue_loaded, "not playing the newest save");
    zox_tst_check(zox_tst_xr_continue_paused3, "the pause menu isn't a 3D list");
    zox_tst_check(zox_tst_xr_continue_resumed, "A on return didn't play on");
}

void zox_tsts_xr_continue(ecs* world) {
    zox_tst_add("xr_continue", zox_tst_xr_continue_ready, zox_tst_xr_continue_check, 180);
    zox_tst_set_setup("xr_continue", zox_tst_xr_continue_setup, 1);
}
