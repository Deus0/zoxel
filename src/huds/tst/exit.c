// tst: exit_menu, clicks exit on the main menu, the app closes itself cleanly
// tst: exit_game, closes the app while playing (like closing the window), it closes cleanly
//  - they pass when the main loop ends and the process exits without crashing or hanging
//      (zox_tst_set_exits, see zox_tst_main_ended)
//  - run: bash bsh/tst.sh exit_menu, bash bsh/tst.sh exit_game
int zox_tst_exit_stage = 0;
int zox_tst_exit_frames = 0;

byte zox_tst_exit_menu_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_exit_frames++;
    if (zox_tst_exit_stage == 0) {
        zox_tst_waiting = "the main menu";
        const entity main_menu = zox_tst_main_menu(world, player, canvas);
        if (!main_menu || zox_tst_exit_frames < 10) {
            return 0;
        }
        if (!zox_tst_menus_click(world, player, main_menu, &button_event_exit_app, "exit")) {
            zox_tst_end("no exit button");
            return 1;
        }
        zox_tst_exit_stage = 1;
    }
    zox_tst_waiting = "the app to close after clicking exit";
    return 0;
}

byte zox_tst_exit_game_ready(ecs* world) {
    const entity player = dbg_player;
    if (!zox_valid(player)) {
        zox_tst_waiting = "the player";
        return 0;
    }
    if (zox_tst_exit_stage == 0) {
        zox_tst_waiting = "playing";
        if (zox_getv(player, PlayerState) != zox_player_state_playing) {
            return 0;
        }
        // lets the world settle a little, streaming and npcs going
        if (++zox_tst_exit_frames < 30) {
            return 0;
        }
        zox_log("[tst] closing the app while playing");
        engine_end();
        zox_tst_exit_stage = 1;
    }
    zox_tst_waiting = "the app to close while playing";
    return 0;
}

void zox_tst_exit_check(ecs* world) {
    (void) world;
}
