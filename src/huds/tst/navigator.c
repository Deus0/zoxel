// tst: navigator, the taskbar's uis stepped through by LB and RB on a gamepad (huds/sys/navigator.c), 2D
//  - every navigator alike: a gamepad here, a headset's grips in xr_navigator (3D, on the simulator)
//  - in a game on the test level, a gamepad (zox_tst_spawn_pad) given to the player, its buttons pressed in the
//    inputs phase as a real one's are (zox_tst_pad_system)
//  - Y opens the game menus (the game going on): the first frame lit, its ui open
//  - RB: the next frame lit, the first's not; LB: back
//  - then RB round every ui: each one opens (not a closing one)
//  - Start pauses the game: its pause menu (not a game menu, no taskbar frame), the selection in it; Start resumes
//  - run: bash bsh/tst.sh navigator
entity zox_tst_pad = 0;
byte zox_tst_pad_held[16];              // by button type (zox_btn_*)
float2 zox_tst_pad_stick = { 0, 0 };    // the left stick, as SDL has it
int zox_tst_nav2_stage = 0;
int zox_tst_nav2_frames = 0;
int zox_tst_nav2_pause = -1;            // the pause menu's frame in the row
int zox_tst_nav2_count = 0;
byte zox_tst_nav2_selected_paused = 0;  // the selection in the pause menu, paused
byte zox_tst_nav2_rb = 0;               // RB lit the next frame, unlit the pause one
byte zox_tst_nav2_lb = 0;               // LB lit the pause one, the selection back in it
int zox_tst_nav2_round = 0;
int zox_tst_nav2_round_open = 0;
char zox_tst_nav2_closed[256] = "";
int zox_tst_nav2_grid_first = -1;       // navigator_grid: the selection's place in its window's NavigationElements
int zox_tst_nav2_grid_right = -1;       // after the stick right
int zox_tst_nav2_grid_down = -1;        // after the stick down
int zox_tst_nav2_grid_columns = 0;

// the gamepad's selection's place among a window's NavigationElements, -1 not in it
static inline int zox_tst_nav2_grid_index(ecs* world, entity window) {
    const entity target = zox_valid(zox_tst_pad) ? zox_getv(zox_tst_pad, Target) : 0;
    entity elements[256];
    const uint count = zox_valid(window) ? zox_get_children_by_id_recursive(world, window, elements, 256, zox_id(NavigationElement), 0) : 0;
    for (uint i = 0; i < count; i++) {
        if (elements[i] == target) {
            return (int) i;
        }
    }
    return -1;
}

// the scripted buttons into the gamepad's zevices, pressed or released this frame on a change
void zox_tst_pad_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    if (!zox_valid(zox_tst_pad)) {
        return;
    }
    byte any = 0;
    iter it2 = zox_children(world, zox_tst_pad);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity e2 = it2.entities[j];
            if (!zox_has(e2, ZeviceButton) || zox_has(e2, ZeviceBumper)) {
                continue;
            }
            const byte type = zox_getv(e2, DeviceButtonType);
            const byte held = type < 16 && zox_tst_pad_held[type];
            const byte old = zox_getv(e2, ZeviceButton);
            const byte was = devices_get_pressed(old);
            byte next = 0;
            if (!was && held) {
                devices_set_pressed_this_frame(&next, 1);
            }
            if (was && !held) {
                devices_set_released_this_frame(&next, 1);
            }
            if (held) {
                devices_set_is_pressed(&next, 1);
            }
            if (next != old) {
                zox_setv(e2, ZeviceButton, next);
            }
            any |= held;
        }
    }
    {
        iter it3 = zox_children(world, zox_tst_pad);
        while (zox_children_next(it3)) {
            for (int j = 0; j < it3.count; j++) {
                if (zox_has(it3.entities[j], ZeviceStick)) {
                    zox_setv(it3.entities[j], ZeviceStick, zox_tst_pad_stick);
                }
            }
        }
        any |= zox_tst_pad_stick.x != 0 || zox_tst_pad_stick.y != 0;
    }
    zox_setv(zox_tst_pad, DeviceHasInput, any);
    zox_sys_on_end();
} zoxd_system(zox_tst_pad_system);

// a gamepad of the player's, no SDL behind it (prefab_gamepad's SdlGameController would be closed, it deleted), as a
//  headset's controllers are made: a device, a Gamepad, a navigator, its buttons and stick
entity zox_tst_spawn_pad(ecs* world, entity player) {
    const entity e = zox_ins(world, prefab_device);
    zox_set_unique_name(e, "tst_gamepad");
    zox_add(e, Gamepad);
    zox_setv(e, Target, 0);
    prefab_add_navigator(world, e);
    zox_set_parent(world, e, player);
    const byte buttons[] = { zox_btn_a, zox_btn_b, zox_btn_y, zox_btn_lb, zox_btn_rb, zox_btn_start };
    for (byte i = 0; i < sizeof(buttons); i++) {
        spawn_device_button(world, prefab_zevice_button, e, buttons[i], i);
    }
    spawn_zevice_stick(world, e, 0, 0);
    return e;
}

static inline void zox_tst_nav2_next() {
    zox_tst_nav2_stage++;
    zox_tst_nav2_frames = 0;
}

static inline byte zox_tst_nav2_lit(ecs* world, entity canvas, int index) {
    entity frames[navigator_frames_max];
    const int count = navigator_frames(world, canvas, frames);
    return index >= 0 && index < count && zox_getv(frames[index], ActiveState);
}

// the gamepad's selection is within the canvas's MenuPaused
static inline byte zox_tst_nav2_selected_in_pause(ecs* world, entity canvas) {
    const entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuPaused));
    const entity target = zox_valid(zox_tst_pad) ? zox_getv(zox_tst_pad, Target) : 0;
    return zox_valid(menu) && zox_valid(target) && zox_get_parent_by_id(world, target, zox_id(Window)) == menu;
}

// the ui open for frame index (not one closing)
static inline entity zox_tst_nav2_open(ecs* world, entity canvas, int index) {
    entity frames[navigator_frames_max];
    const int count = navigator_frames(world, canvas, frames);
    if (index < 0 || index >= count) {
        return 0;
    }
    const entity id = zox_getv(frames[index], TaskbarWindowID);
    entity found = 0;
    iter it = zox_children(world, canvas);
    while (zox_children_next(it)) {
        for (int j = 0; j < it.count; j++) {
            if (zox_has_id(it.entities[j], id) && !zox_has(it.entities[j], UIClosing)) {
                found = it.entities[j];
            }
        }
    }
    return found;
}

byte zox_tst_nav2_menu_playing = 0;        // in the game menus the game wasn't paused
byte zox_tst_nav2_pause_menu = 0;          // Start: the game paused, its pause menu up, the selection in it
byte zox_tst_nav2_pause_off_taskbar = 0;   // the pause menu has no taskbar frame
byte zox_tst_nav2_resumed = 0;             // Start again: playing, the pause menu gone

byte zox_tst_navigator_ready(ecs* world) {
    if (zox_tst_nav2_stage == 0) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_pad = zox_tst_spawn_pad(world, zox_players[0]);
        zox_tst_nav2_next();
        return 0;
    }
    const entity player = zox_players[0];
    const entity canvas = zox_get_link(world, player, CanvasLink);
    const entity game = zox_get_link(world, player, GameLink);
    const int f = ++zox_tst_nav2_frames;
    zox_tst_progress(zox_tst_nav2_stage * 1000 + f);
    memset(zox_tst_pad_held, 0, sizeof(zox_tst_pad_held));
    zox_tst_pad_stick = float2_zero;
    switch (zox_tst_nav2_stage) {
        case 1:
            zox_tst_waiting = "Y opening the game menus";
            // a first touch (the stick) switches the player to the gamepad, then Y
            {
                static int moded = 0;
                if (!moded) {
                    zox_tst_pad_stick = (float2) { 0, (f % 20) < 4 ? 0.5f : 0 };
                    if (zox_getv(player, DeviceMode) == zox_device_mode_gamepad) {
                        moded = f;
                    }
                    return f > 600;
                }
                zox_tst_pad_held[zox_btn_y] = f >= moded + 20 && f < moded + 24;
            }
            if (f > 60 && zox_getv(player, PlayerState) == zox_player_state_paused) {
                entity frames[navigator_frames_max];
                zox_tst_nav2_count = navigator_frames(world, canvas, frames);
                zox_tst_nav2_pause = 0;     // the first game menu opens
                zox_tst_nav2_menu_playing = zox_getv(game, GameState) == zox_game_state_playing;
                // (the first, the mirror, has nothing to select)
                if (zox_tst_nav2_lit(world, canvas, 0) || f > 200) {
                    zox_tst_nav2_selected_paused = zox_tst_nav2_lit(world, canvas, 0);
                    for (int i = 0; i < zox_tst_nav2_count; i++) {
                        if (zox_getv(frames[i], TaskbarWindowID) == zox_id(MenuPaused)) {
                            zox_tst_nav2_pause_off_taskbar = 2;     // it has one
                        }
                    }
                    zox_tst_nav2_next();
                }
            }
            return f > 600;
        case 2:
            zox_tst_waiting = "RB";
            zox_tst_pad_held[zox_btn_rb] = f >= 2 && f < 6;
            if (f == 30) {
                zox_tst_nav2_rb = zox_tst_nav2_count > 1 && zox_tst_nav2_lit(world, canvas, 1) && !zox_tst_nav2_lit(world, canvas, 0);
                zox_tst_nav2_next();
            }
            return 0;
        case 3:
            zox_tst_waiting = "LB";
            zox_tst_pad_held[zox_btn_lb] = f >= 2 && f < 6;
            if (f >= 30) {
                zox_tst_nav2_lb = zox_tst_nav2_lit(world, canvas, 0) && !zox_tst_nav2_lit(world, canvas, 1);
                zox_tst_nav2_next();
            }
            return 0;
        case 4:
            zox_tst_waiting = "RB round every ui";
            {
                const int step = (f - 1) / 40, at = (f - 1) % 40;
                if (step >= zox_tst_nav2_count) {
                    zox_tst_nav2_next();
                    return 0;
                }
                zox_tst_pad_held[zox_btn_rb] = at >= 2 && at < 6;
                if (at == 30) {
                    const int index = (1 + step) % zox_tst_nav2_count;
                    zox_tst_nav2_round++;
                    if (zox_valid(zox_tst_nav2_open(world, canvas, index))) {
                        zox_tst_nav2_round_open++;
                    } else {
                        entity frames[navigator_frames_max];
                        navigator_frames(world, canvas, frames);
                        const size_t used = strlen(zox_tst_nav2_closed);
                        snprintf(zox_tst_nav2_closed + used, sizeof(zox_tst_nav2_closed) - used, " [%s]",
                            zox_getn(zox_getv(frames[index], TaskbarWindowID)));
                    }
                }
            }
            return 0;
        case 5:
            zox_tst_waiting = "Start pausing the game";
            zox_tst_pad_held[zox_btn_start] = f >= 2 && f < 6;
            if (f > 30 && zox_getv(game, GameState) == zox_game_state_paused &&
                (zox_tst_nav2_selected_in_pause(world, canvas) || f > 200)) {
                zox_tst_nav2_pause_menu = zox_tst_nav2_selected_in_pause(world, canvas);
                zox_tst_nav2_next();
            }
            return f > 300;
        case 6:
            zox_tst_waiting = "Start resuming";
            zox_tst_pad_held[zox_btn_start] = f >= 2 && f < 6;
            if (f > 60 && zox_getv(game, GameState) == zox_game_state_playing &&
                !zox_valid(zox_get_child_by_id(world, canvas, zox_id(MenuPaused)))) {
                zox_tst_nav2_resumed = 1;
                return 1;
            }
            return f > 300;
    }
    return 0;
}

void zox_tst_navigator_check(ecs* world) {
    (void) world;
    zox_log("[tst] navigator: [%i] frames, the first selected [%i], playing in the menus [%i], RB [%i], LB [%i], round [%i] of [%i] opened, not:%s; paused menu [%i], resumed [%i]",
        zox_tst_nav2_count, zox_tst_nav2_selected_paused, zox_tst_nav2_menu_playing, zox_tst_nav2_rb, zox_tst_nav2_lb,
        zox_tst_nav2_round_open, zox_tst_nav2_round, zox_tst_nav2_closed, zox_tst_nav2_pause_menu, zox_tst_nav2_resumed);
    zox_tst_check(zox_tst_nav2_selected_paused, "Y: the first game menu isn't open");
    zox_tst_check(zox_tst_nav2_menu_playing, "in the game menus the game was paused");
    zox_tst_check(zox_tst_nav2_pause_off_taskbar != 2, "the pause menu is a game menu (a taskbar frame)");
    zox_tst_check(zox_tst_nav2_rb, "RB didn't step to the next ui");
    zox_tst_check(zox_tst_nav2_lb, "LB didn't step back to the first ui");
    zox_tst_check(zox_tst_nav2_round == zox_tst_nav2_count && zox_tst_nav2_round_open == zox_tst_nav2_count,
        "RB round the taskbar opened [%i] of [%i], not:%s", zox_tst_nav2_round_open, zox_tst_nav2_count, zox_tst_nav2_closed);
    zox_tst_check(zox_tst_nav2_pause_menu, "Start: the game didn't pause with its pause menu, the selection in it");
    zox_tst_check(zox_tst_nav2_resumed, "Start again didn't resume the game, the pause menu gone");
}

// tst: navigator_grid, a gamepad's stick steps a grid (element_navigation_system): a window of 4 x 3 frames, their
//  icons NavigationElements as a datagrid's; the first selected, the stick right one along, down a row (4); the
//  stick's x as SDL's (right is -x); the selected icon's tooltip under it (tooltip_navigated_system), not at the
//  still mouse
#define zox_tst_nav2_grid_size ((byte2) { 4, 3 })
entity zox_tst_nav2_grid = 0;
int2 zox_tst_nav2_tip_off = { 999, 999 };   // the tooltip's top right from the selected icon's bottom right

static entity zox_tst_nav2_grid_spawn(ecs* world, entity canvas) {
    const byte2 cells = zox_tst_nav2_grid_size;
    const int frame = 32;
    const int2 size = calculate_grid_size(cells, frame, byte2_single(2), byte2_single(4));
    const entity3 e3 = spawn_window(world, prefab_window, prefab_grid, "Grid", canvas, int2_zero, size, float2_half,
        8, (byte2) { 4, 2 }, NULL);
    zox_setv(e3.z, GridSize, cells);
    zox_setv(e3.z, GridPadding, byte2_single(2));
    zox_setv(e3.z, GridMargins, byte2_single(4));
    zox_add(e3.x, NavigationWindow);
    entity first = 0;
    for (int i = 0; i < cells.x * cells.y; i++) {
        const entity3 spawn = spawn_frame(world, prefab_frame, prefab_icon, 0, e3.z, int2_zero, int2_single(frame),
            int2_single(frame - 8), 0, i);
        zox_add(spawn.y, NavigationElement);
        zox_add_tooltip_text(world, spawn.y, "a cell");
        if (!first) {
            first = spawn.y;
        }
    }
    zox_tst_nav2_grid = e3.x;
    return first;
}

byte zox_tst_navigator_grid_ready(ecs* world) {
    if (zox_tst_nav2_stage == 0) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_pad = zox_tst_spawn_pad(world, zox_players[0]);
        zox_tst_nav2_next();
        return 0;
    }
    const entity player = zox_players[0];
    const int f = ++zox_tst_nav2_frames;
    zox_tst_progress(zox_tst_nav2_stage * 1000 + f);
    memset(zox_tst_pad_held, 0, sizeof(zox_tst_pad_held));
    zox_tst_pad_stick = float2_zero;
    static entity first = 0;
    switch (zox_tst_nav2_stage) {
        case 1:
            zox_tst_waiting = "the gamepad's mode";
            zox_tst_pad_held[zox_btn_y] = (f % 20) < 4;
            if (zox_getv(player, DeviceMode) == zox_device_mode_gamepad) {
                first = zox_tst_nav2_grid_spawn(world, zox_get_link(world, player, CanvasLink));
                zox_tst_nav2_next();
            }
            return f > 600;
        case 2:
            zox_tst_waiting = "the grid's first icon selected, the stick right, then down";
            if (f == 20) {
                raycaster_select_element(world, zox_tst_pad, first);
            }
            if (f == 30) {
                zox_tst_nav2_grid_first = zox_tst_nav2_grid_index(world, zox_tst_nav2_grid);
            }
            zox_tst_pad_stick = (f >= 32 && f < 38) ? (float2) { -1, 0 } : (f >= 62 && f < 68) ? (float2) { 0, -1 } : float2_zero;
            if (f == 58) {
                zox_tst_nav2_grid_right = zox_tst_nav2_grid_index(world, zox_tst_nav2_grid);
            }
            if (f == 90) {
                zox_tst_nav2_grid_down = zox_tst_nav2_grid_index(world, zox_tst_nav2_grid);
                const entity canvas = zox_get_link(world, player, CanvasLink);
                const entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
                const entity icon = zox_getv(zox_tst_pad, Target);
                CanvasViewport viewport;
                if (zox_valid(tooltip) && zox_valid(icon) && get_canvas_viewport(world, canvas, &viewport) &&
                    zox_has(tooltip, Position2) && zox_has(icon, Position2)) {
                    const int4 t = get_element_pointer_bounds(world, &viewport, tooltip, zox_getv(tooltip, Position2), zox_getv(tooltip, LayoutSize));
                    const int4 b = get_element_pointer_bounds(world, &viewport, icon, zox_getv(icon, Position2), zox_getv(icon, LayoutSize));
                    zox_tst_nav2_tip_off = (int2) { t.y - b.y, t.w - b.z };
                }
                return 1;
            }
            return 0;
    }
    return 0;
}

void zox_tst_navigator_grid_check(ecs* world) {
    (void) world;
    const int columns = zox_tst_nav2_grid_size.x;
    zox_log("[tst] navigator_grid: selected at [%i], right [%i], down [%i]", zox_tst_nav2_grid_first,
        zox_tst_nav2_grid_right, zox_tst_nav2_grid_down);
    zox_tst_check(zox_tst_nav2_grid_first == 0, "the first icon isn't selected [%i]", zox_tst_nav2_grid_first);
    zox_tst_check(zox_tst_nav2_grid_right == 1, "the stick right went to [%i], not [1]", zox_tst_nav2_grid_right);
    zox_log("[tst] navigator_grid: the tooltip's top right from the icon's bottom right [%i %i]", zox_tst_nav2_tip_off.x, zox_tst_nav2_tip_off.y);
    zox_tst_check(abs(zox_tst_nav2_tip_off.x) <= 3 && zox_tst_nav2_tip_off.y <= 0 && zox_tst_nav2_tip_off.y >= -8,
        "the tooltip isn't under the selected icon [%i %i]", zox_tst_nav2_tip_off.x, zox_tst_nav2_tip_off.y);
    zox_tst_check(zox_tst_nav2_grid_down == 1 + columns, "the stick down went to [%i], not a row down [%i]",
        zox_tst_nav2_grid_down, 1 + columns);
}

void zox_tsts_navigator(ecs* world) {
    zox_tst_add("navigator_grid", zox_tst_navigator_grid_ready, zox_tst_navigator_grid_check, 120);
    zox_tst_set_setup("navigator_grid", zox_tst_level_setup, 0);
    zox_system_1(zox_tst_pad_system, zoxp_inputs_extract, 0);
    zox_tst_add("navigator", zox_tst_navigator_ready, zox_tst_navigator_check, 180);
    zox_tst_set_setup("navigator", zox_tst_level_setup, 0);
}
