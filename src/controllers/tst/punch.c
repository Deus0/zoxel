// tst: resume_no_punch, clicking a ui button that resumes play doesn't make the character attack
//  - pauses, then presses and releases the pause menu's return button with the fake mouse
//  - the character must never get TriggerActionA (attack) while resuming and after
//      (buttons click on release, when they clicked on press the still held button punched,
//      the same for the last dialogue option ending a dialogue)
//  - run: bash bsh/tst.sh resume_no_punch
#define zox_tst_punch_hold_frames 4
#define zox_tst_punch_watch_frames 60
int zox_tst_punch_stage = 0;
int zox_tst_punch_frames = 0;
int2 zox_tst_punch_on_return;
int zox_tst_punch_attacks = 0;

byte zox_tst_resume_no_punch_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    const entity character = zox_valid(player) ? zox_get_link(world, player, CharacterLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_punch_frames++;
    // from the press on, watches for an attack every frame
    if (zox_tst_punch_stage >= 3 && zox_valid(character) && zox_has(character, TriggerActionA) && zox_getv(character, TriggerActionA)) {
        zox_tst_punch_attacks++;
    }
    switch (zox_tst_punch_stage) {
        case 0:
            zox_tst_waiting = "playing";
            if (zox_getv(player, PlayerState) != zox_player_state_playing || zox_tst_punch_frames < 10) {
                return 0;
            }
            button_event_pause_game(world, (ClickEventData) { .clicker = player });
            break;
        case 1: {
            zox_tst_waiting = "the pause menu";
            const entity paused = zox_get_child_by_id(world, canvas, zox_id(MenuPaused));
            if (!zox_valid(paused) || zox_tst_punch_frames < 10) {
                return 0;
            }
            const entity button = zox_tst_find_click(world, paused, &button_event_return_to_game);
            if (!button || !zox_tst_ui_pointer_position(world, canvas, button, &zox_tst_punch_on_return)) {
                zox_tst_check(0, "pause menu has no return button on screen");
                return 1;
            }
            break;
        }
        case 2:
            // a left click on nothing, so the player uses the mouse (raycasting the ui)
            zox_tst_waiting = "a click on nothing";
            zox_tst_mouse((int2) { 4, 4 }, zox_tst_punch_frames < zox_tst_punch_hold_frames);
            if (zox_tst_punch_frames < 2 * zox_tst_punch_hold_frames) {
                return 0;
            }
            break;
        case 3:
            zox_tst_waiting = "pressing return to game";
            // where the button is now (the menu settles its layout after spawning)
            if (zox_tst_punch_frames == 1) {
                const entity paused = zox_get_child_by_id(world, canvas, zox_id(MenuPaused));
                const entity button = zox_valid(paused) ? zox_tst_find_click(world, paused, &button_event_return_to_game) : 0;
                if (!button || !zox_tst_ui_pointer_position(world, canvas, button, &zox_tst_punch_on_return)) {
                    zox_tst_check(0, "pause menu has no return button on screen");
                    return 1;
                }
            }
            zox_tst_mouse(zox_tst_punch_on_return, zox_tst_punch_frames > zox_tst_punch_hold_frames);
            if (zox_tst_punch_frames < 2 * zox_tst_punch_hold_frames) {
                return 0;
            }
            {
                const entity target = zox_tst_mouse_target(world, player);
                zox_log("[tst] pressing return to game at [%ix%i], over [%s]", zox_tst_punch_on_return.x, zox_tst_punch_on_return.y,
                    zox_valid(target) ? zox_getn(target) : "nothing");
            }
            zox_tst_check(zox_getv(player, PlayerState) == zox_player_state_paused, "pressing return to game resumed before release");
            break;
        case 4:
            zox_tst_waiting = "releasing return to game, then playing";
            zox_tst_mouse(zox_tst_punch_on_return, 0);
            if (zox_getv(player, PlayerState) != zox_player_state_playing) {
                return 0;
            }
            break;
        default:
            zox_tst_waiting = "a while after resuming";
            zox_tst_mouse(zox_tst_punch_on_return, 0);
            if (zox_tst_punch_frames < zox_tst_punch_watch_frames) {
                return 0;
            }
            zox_tst_check(zox_tst_punch_attacks == 0, "the character attacked on [%i] frames after clicking return to game", zox_tst_punch_attacks);
            zox_tst_mouse_end();
            return 1;
    }
    zox_tst_punch_stage++;
    zox_tst_punch_frames = 0;
    return 0;
}

void zox_tst_resume_no_punch_check(ecs* world) {
    (void) world;
    zox_tst_mouse_end();
}
