// tst: mouse_leave, the mouse leaving the window (off the canvas) unhovers what it was over: in game a tooltip stuck
//  when the mouse left it (sdl keeps the mouse's last place inside the window, element_raycast_system kept its target
//  while the pointer was off the canvas)
//  - on the main menu, hovers the new game button with the fake mouse (it's the pointer's target)
//  - moves it off the canvas (int2_hidden, where mouse_input_system puts a mouse out of the window)
//  - checks: hovering, it's the target; off the canvas, nothing is
//  - run: bash bsh/tst.sh mouse_leave
int zox_tst_ml_stage = 0;
int zox_tst_ml_frames = 0;
int2 zox_tst_ml_on_button;
entity zox_tst_ml_button = 0;
byte zox_tst_ml_hovered = 0, zox_tst_ml_left = 0;

byte zox_tst_mouse_leave_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_ml_frames++;
    const entity main_menu = zox_tst_menu(world, canvas, zox_id(MenuMain));
    const entity target = zox_tst_mouse_target(world, player);
    if (zox_tst_ml_stage == 0) {
        // clicks until the start menu opens the main menu, then waits for its layout
        zox_tst_waiting = "the main menu";
        static int clicks = 0;
        zox_tst_mouse((int2) { 100, 100 }, !main_menu && (++clicks / 4) % 2);
        if (!main_menu) {
            zox_tst_ml_frames = 0;
            return 0;
        }
        if (zox_tst_ml_frames < 10) {
            return 0;
        }
        zox_tst_ml_button = zox_tst_find_click(world, main_menu, &button_event_new_game);
        if (!zox_tst_ml_button || !zox_tst_ui_pointer_position(world, canvas, zox_tst_ml_button, &zox_tst_ml_on_button)) {
            zox_tst_check(0, "no new game button to hover");
            zox_tst_mouse_end();
            return 1;
        }
        zox_tst_ml_stage = 1;
        zox_tst_ml_frames = 0;
        return 0;
    }
    if (zox_tst_ml_stage == 1) {
        zox_tst_waiting = "hovering new game";
        zox_tst_mouse(zox_tst_ml_on_button, 0);
        if (zox_tst_ml_frames < 4) {
            return 0;
        }
        zox_tst_ml_hovered = target == zox_tst_ml_button;
        zox_tst_ml_stage = 2;
        zox_tst_ml_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the mouse off the canvas";
    zox_tst_mouse(int2_hidden, 0);
    if (zox_tst_ml_frames < 4) {
        return 0;
    }
    zox_tst_ml_left = !target;
    zox_tst_mouse_end();
    return 1;
}

void zox_tst_mouse_leave_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_ml_hovered, "hovering new game didn't target it");
    zox_tst_check(zox_tst_ml_left, "off the canvas, new game is still the target (its tooltip would stay)");
}
