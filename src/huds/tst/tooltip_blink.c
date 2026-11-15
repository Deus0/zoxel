// tst: tooltip_blink, a ui selected and deselected in the same frame (a fast mouse flick over it) leaves no tooltip
//  - the deselect's unlink ran before the select's deferred link landed, the link stayed and its text stuck
//  - on the main menu: a button with a tooltip gets its select and deselect triggers in one frame, as the raycast
//      sets them, then a few frames later the tooltip must have no text and no link
//  - run: bash bsh/tst.sh tooltip_blink
#define zox_tst_tb_rounds 10
int zox_tst_tb_stage = 0;
int zox_tst_tb_frames = 0;
int zox_tst_tb_round = 0;
int zox_tst_tb_stuck = 0;
entity zox_tst_tb_button = 0;

byte zox_tst_tooltip_blink_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_tb_frames++;
    const entity main_menu = zox_tst_menu(world, canvas, zox_id(MenuMain));
    if (zox_tst_tb_stage == 0) {
        // clicks until the start menu opens the main menu, like a player
        zox_tst_waiting = "a mouse click to open the main menu";
        zox_tst_mouse((int2) { 100, 100 }, (zox_tst_tb_frames / 4) % 2);
        if (main_menu) {
            zox_tst_tb_stage = 1;
            zox_tst_tb_frames = 0;
        }
        return 0;
    }
    const entity tooltip = zox_tst_menu(world, canvas, zox_id(Tooltip));
    if (zox_tst_tb_stage == 1) {
        zox_tst_waiting = "the main menu's layout";
        // the mouse off in a corner, so the raycast leaves the buttons alone
        zox_tst_mouse((int2) { 1, 1 }, 0);
        if (!main_menu || zox_tst_tb_frames < 10) {
            return 0;
        }
        zox_tst_tb_button = zox_tst_find_click(world, main_menu, &button_event_new_game);
        if (!zox_valid(zox_tst_tb_button) || !zox_has(zox_tst_tb_button, TooltipText) || !tooltip) {
            zox_tst_check(0, "no main menu button with a tooltip, or no tooltip");
            return 1;
        }
        zox_tst_tb_stage = 2;
        zox_tst_tb_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the tooltip blinks";
    zox_tst_progress(zox_tst_tb_round);
    const entity button = zox_tst_tb_button;
    if (zox_tst_tb_stage == 2) {
        // selected and deselected in one frame, as set_raycast_target_children sets them
        zox_setm(button, SelectState, zox_state_select_trigger);
        zox_add(button, TriggerSelect);
        zox_setm(button, SelectState, zox_state_deselect_trigger);
        zox_add(button, TriggerDeselect);
        zox_tst_tb_stage = 3;
        zox_tst_tb_frames = 0;
        return 0;
    }
    if (zox_tst_tb_frames < 10) {
        return 0;
    }
    const TextData* text = zox_get(tooltip, TextData);
    if (text->length || zox_is_linked_any(world, tooltip, TooltipLink)) {
        zox_tst_tb_stuck++;
        zox_loge("[tst] round [%i]: the tooltip stuck with [%i] characters", zox_tst_tb_round, text->length);
    }
    zox_tst_tb_round++;
    zox_tst_tb_stage = 2;
    zox_tst_tb_frames = 0;
    if (zox_tst_tb_round < zox_tst_tb_rounds) {
        return 0;
    }
    zox_tst_check(zox_tst_tb_stuck == 0, "the tooltip stuck after [%i] of [%i] blinks", zox_tst_tb_stuck, zox_tst_tb_rounds);
    zox_tst_mouse_end();
    return 1;
}

void zox_tst_tooltip_blink_check(ecs* world) {
    (void) world;
    zox_tst_mouse_end();
}
