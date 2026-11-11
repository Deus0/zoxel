// tst: menu_flow, walks the menus like a player, from the start menu into a game and back out
//  - start > main > options > back > main > new game > enter realm > playing
//  - pause > return to game > pause > end game > main menu > new game > playing again
//  - each step: exactly one menu is open, the last one was closed, and the mouse lock suits it
//      (unlocked in menus and paused, locked while playing)
//  - clicks call the buttons' click events directly, mouse_click tests the mouse path
//  - run: bash bsh/tst.sh menu_flow
//
// tst: ui_leaks, opens and closes the options menu many times from the main menu
//  - measures the canvas while options is open (its contents are always the same,
//      main menu labels are random), the first cycles warm up, then it's the baseline
//  - after every cycle, the canvas's entity count, live gpu buffers and textures match the baseline
//  - run: bash bsh/tst.sh ui_leaks
#define zox_tst_menus_settle_frames 5
#define zox_tst_leaks_warmup 3
#define zox_tst_leaks_cycles 30
int zox_tst_menus_stage = 0;
int zox_tst_menus_frames = 0;
byte zox_tst_menus_quiet = 0;    // ui_leaks clicks too often to log each one

static inline void zox_tst_menus_next() {
    zox_tst_menus_stage++;
    zox_tst_menus_frames = 0;
}

// menus open on the canvas
int zox_tst_menus_open(ecs* world, entity canvas) {
    int count = 0;
    iter it2 = zox_children(world, canvas);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity e = it2.entities[j];
            if (zox_has(e, MenuMain) ||
                zox_has(e, MenuOptions) ||
                zox_has(e, MenuPaused) ||
                zox_has(e, MenuNewRealm) ||
                zox_has(e, MenuLoad) ||
                zox_has(e, MenuRealm))
            {
                count++;
            }
        }
    }
    return count;
}

static inline entity zox_tst_menu(ecs* world, entity canvas, entity id) {
    const entity e = zox_get_child_by_id(world, canvas, id);
    return zox_valid(e) ? e : 0;
}

// the main menu, pressing any key on the start menu first, like a player
entity zox_tst_main_menu(ecs* world, entity player, entity canvas) {
    if (zox_valid(zox_get_link(world, player, MenuStart))) {
        menu_start_triggered(world, player, canvas);
        return 0;
    }
    return zox_tst_menu(world, canvas, zox_id(MenuMain));
}

static inline byte zox_tst_game_state(ecs* world, entity player) {
    const entity game = zox_get_link(world, player, GameLink);
    return zox_valid(game) ? zox_getv(game, GameState) : 0;
}

// clicks a menu's button, failing the check if it's missing
byte zox_tst_menus_click(
    ecs* world,
    entity player,
    entity menu,
    void (*on_click)(ecs*, ClickEventData),
    const char* label)
{
    const entity button = zox_tst_find_click(world, menu, on_click);
    zox_tst_check(button, "[%s] button not found in [%s]", label, zox_getn(menu));
    if (!button) {
        return 0;
    }
    if (!zox_tst_menus_quiet) {
        zox_log("[tst] clicking [%s]", label);
    }
    zox_tst_click(world, player, button);
    return 1;
}

byte zox_tst_menu_flow_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_menus_frames++;
    if (zox_tst_menus_frames < zox_tst_menus_settle_frames) {
        return 0;
    }
    const entity main_menu = zox_tst_menu(world, canvas, zox_id(MenuMain));
    const entity options = zox_tst_menu(world, canvas, zox_id(MenuOptions));
    const entity new_realm = zox_tst_menu(world, canvas, zox_id(MenuNewRealm));
    const entity paused = zox_tst_menu(world, canvas, zox_id(MenuPaused));
    const int menus = zox_tst_menus_open(world, canvas);
    const byte state = zox_tst_game_state(world, player);
    const int lock = zox_tst_mouse_lock(world, player);
    // the player follows the game state after a delay (i.e. resume_time), so waits on both
    const byte playing = state == zox_game_state_playing &&
        zox_getv(player, PlayerState) == zox_player_state_playing;
    switch (zox_tst_menus_stage) {
        case 0:
            zox_tst_waiting = "the main menu";
            if (!zox_tst_main_menu(world, player, canvas)) {
                return 0;
            }
            zox_tst_check(menus == 1, "main menu: [%i] menus open, expected 1", menus);
            zox_tst_check(lock == 0, "main menu: mouse lock is [%i], expected unlocked", lock);
            if (!zox_tst_menus_click(world, player, main_menu, &button_event_menu_options, "options")) {
                return 1;
            }
            break;
        case 1:
            zox_tst_waiting = "the options menu";
            if (!options) {
                return 0;
            }
            zox_tst_check(!main_menu, "options: main menu wasn't closed");
            zox_tst_check(menus == 1, "options: [%i] menus open, expected 1", menus);
            if (!zox_tst_menus_click(world, player, options, &button_event_menu_main, "options back")) {
                return 1;
            }
            break;
        case 2:
            zox_tst_waiting = "the main menu, back from options";
            if (!main_menu) {
                return 0;
            }
            zox_tst_check(!options, "back: options menu wasn't closed");
            zox_tst_check(menus == 1, "back: [%i] menus open, expected 1", menus);
            if (!zox_tst_menus_click(world, player, main_menu, &button_event_new_game, "new game")) {
                return 1;
            }
            break;
        case 3:
            zox_tst_waiting = "the new realm menu";
            if (!new_realm) {
                return 0;
            }
            zox_tst_check(!main_menu, "new game: main menu wasn't closed");
            zox_tst_check(menus == 1, "new game: [%i] menus open, expected 1", menus);
            if (!zox_tst_menus_click(world, player, new_realm, &on_confirmed_new_realm, "enter realm")) {
                return 1;
            }
            break;
        case 4:
            zox_tst_waiting = "the game to start playing";
            if (!playing) {
                return 0;
            }
            break;
        case 5:
            // settled in game, the player state has caught up with the game state
            zox_tst_waiting = "the game to settle";
            zox_tst_check(menus == 0, "playing: [%i] menus open, expected none", menus);
            zox_tst_check(lock == 1, "playing: mouse lock is [%i], expected locked", lock);
            zox_log("[tst] pausing");
            button_event_pause_game(world, (ClickEventData) { .clicker = player });
            break;
        case 6:
            zox_tst_waiting = "the pause menu";
            if (!paused) {
                return 0;
            }
            zox_tst_check(state == zox_game_state_paused, "paused: game state is [%i], expected paused", state);
            zox_tst_check(menus == 1, "paused: [%i] menus open, expected 1", menus);
            zox_tst_check(lock == 0, "paused: mouse lock is [%i], expected unlocked", lock);
            if (!zox_tst_menus_click(world, player, paused, &button_event_return_to_game, "return to game")) {
                return 1;
            }
            break;
        case 7:
            zox_tst_waiting = "the game to resume";
            if (!playing || paused) {
                return 0;
            }
            break;
        case 8:
            zox_tst_waiting = "the game to settle after resuming";
            zox_tst_check(menus == 0, "resumed: [%i] menus open, expected none", menus);
            zox_tst_check(!zox_tst_menu(world, canvas, zox_id(Taskbar)), "resumed: taskbar wasn't closed");
            zox_tst_check(lock == 1, "resumed: mouse lock is [%i], expected locked", lock);
            zox_log("[tst] pausing again");
            button_event_pause_game(world, (ClickEventData) { .clicker = player });
            break;
        case 9:
            zox_tst_waiting = "the pause menu, again";
            if (!paused) {
                return 0;
            }
            if (!zox_tst_menus_click(world, player, paused, &button_event_end_game, "end game")) {
                return 1;
            }
            break;
        case 10:
            zox_tst_waiting = "the main menu, after ending the game";
            if (!main_menu) {
                return 0;
            }
            zox_tst_check(!paused, "ended: pause menu wasn't closed");
            zox_tst_check(!zox_tst_menu(world, canvas, zox_id(Taskbar)), "ended: taskbar wasn't closed");
            zox_tst_check(menus == 1, "ended: [%i] menus open, expected 1", menus);
            zox_tst_check(lock == 0, "ended: mouse lock is [%i], expected unlocked", lock);
            zox_tst_check(state == zox_game_none,
                "ended: game state is [%i], expected none [%i] (stuck after deleting the realm?)", state, zox_game_none);
            {
                // the camera went back to the main menu's spot (player_state_game_end)
                const entity camera = zox_get_link(world, player, CameraLink);
                const float3 camera_position = zox_valid(camera) ? zox_getv(camera, Position3D) : float3_zero;
                const float3 offset = float3_sub(camera_position, main_menu_camera_position);
                zox_tst_check(zox_valid(camera) && !zox_valid(zox_get_parent(world, camera)), "ended: the camera is still parented");
                zox_tst_check(fabsf(offset.x) + fabsf(offset.y) + fabsf(offset.z) < 0.01f,
                    "ended: the camera is at [%.2fx%.2fx%.2f], not back at the main menu's spot",
                    camera_position.x, camera_position.y, camera_position.z);
            }
            {
                const entity game = zox_get_link(world, player, GameLink);
                zox_log("[tst] ended: game state [%i] target [%i], player state [%i]",
                    state, zox_getv(game, GameStateTarget), zox_getv(player, PlayerState));
            }
            // a second game, from the main menu we returned to
            if (!zox_tst_menus_click(world, player, main_menu, &button_event_new_game, "new game, again")) {
                return 1;
            }
            break;
        case 11:
            zox_tst_waiting = "the new realm menu, again";
            if (!new_realm) {
                return 0;
            }
            if (!zox_tst_menus_click(world, player, new_realm, &on_confirmed_new_realm, "enter realm, again")) {
                return 1;
            }
            break;
        case 12:
            zox_tst_waiting = "the second game to start playing";
            if (!playing) {
                return 0;
            }
            break;
        default:
            zox_tst_waiting = "the second game to settle";
            zox_tst_check(menus == 0, "second game: [%i] menus open, expected none", menus);
            zox_tst_check(lock == 1, "second game: mouse lock is [%i], expected locked", lock);
            return 1;
    }
    zox_tst_menus_next();
    return 0;
}

void zox_tst_menu_flow_check(ecs* world) {
    (void) world;
}

// ui_leaks
int zox_tst_leaks_cycle = 0;
int zox_tst_leaks_entities = 0;
int zox_tst_leaks_buffers = 0;
int zox_tst_leaks_textures = 0;
int zox_tst_leaks_failed_cycles = 0;

byte zox_tst_ui_leaks_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_menus_frames++;
    if (zox_tst_menus_frames < zox_tst_menus_settle_frames) {
        return 0;
    }
    if (zox_tst_menus_stage == 0) {
        zox_tst_menus_quiet = 1;
        zox_tst_waiting = "the main menu";
        const entity main_menu = zox_tst_main_menu(world, player, canvas);
        if (!main_menu) {
            return 0;
        }
        if (!zox_tst_menus_click(world, player, main_menu, &button_event_menu_options, "options")) {
            return 1;
        }
        zox_tst_menus_next();
        return 0;
    }
    zox_tst_progress(zox_tst_leaks_cycle);
    if (zox_tst_menus_stage == 1) {
        // options is open
        zox_tst_waiting = "the options menu";
        const entity options = zox_tst_menu(world, canvas, zox_id(MenuOptions));
        if (!options || zox_tst_menu(world, canvas, zox_id(MenuMain))) {
            return 0;
        }
        const int entities = zox_tst_count_descendants(world, canvas);
        if (zox_tst_leaks_cycle == zox_tst_leaks_warmup) {
            zox_tst_leaks_entities = entities;
            zox_tst_leaks_buffers = zox_gpu_buffers_live;
            zox_tst_leaks_textures = zox_gpu_textures_live;
            zox_log("[tst] baseline: canvas entities [%i] gpu buffers [%i] textures [%i]",
                entities, zox_gpu_buffers_live, zox_gpu_textures_live);
        } else if (zox_tst_leaks_cycle > zox_tst_leaks_warmup) {
            const byte leaked =
                entities != zox_tst_leaks_entities ||
                zox_gpu_buffers_live != zox_tst_leaks_buffers ||
                zox_gpu_textures_live != zox_tst_leaks_textures;
            if (leaked) {
                zox_tst_leaks_failed_cycles++;
            }
            // logs the first leak, and the end state
            if ((leaked && zox_tst_leaks_failed_cycles == 1) || zox_tst_leaks_cycle == zox_tst_leaks_cycles) {
                zox_log("[tst] cycle [%i]: canvas entities [%i] gpu buffers [%i] textures [%i]",
                    zox_tst_leaks_cycle, entities, zox_gpu_buffers_live, zox_gpu_textures_live);
            }
        }
        if (zox_tst_leaks_cycle == zox_tst_leaks_cycles) {
            zox_tst_check(entities == zox_tst_leaks_entities,
                "canvas entities [%i] after [%i] cycles, baseline [%i]",
                entities, zox_tst_leaks_cycles - zox_tst_leaks_warmup, zox_tst_leaks_entities);
            zox_tst_check(zox_gpu_buffers_live == zox_tst_leaks_buffers,
                "gpu buffers [%i] after [%i] cycles, baseline [%i]",
                zox_gpu_buffers_live, zox_tst_leaks_cycles - zox_tst_leaks_warmup, zox_tst_leaks_buffers);
            zox_tst_check(zox_gpu_textures_live == zox_tst_leaks_textures,
                "gpu textures [%i] after [%i] cycles, baseline [%i]",
                zox_gpu_textures_live, zox_tst_leaks_cycles - zox_tst_leaks_warmup, zox_tst_leaks_textures);
            zox_tst_check(zox_tst_leaks_failed_cycles == 0,
                "[%i] of [%i] cycles didn't match the baseline",
                zox_tst_leaks_failed_cycles, zox_tst_leaks_cycles - zox_tst_leaks_warmup);
            return 1;
        }
        if (!zox_tst_menus_click(world, player, options, &button_event_menu_main, "options back")) {
            return 1;
        }
        zox_tst_menus_stage = 2;
        zox_tst_menus_frames = 0;
        return 0;
    }
    // main menu is open
    zox_tst_waiting = "the main menu, back from options";
    const entity main_menu = zox_tst_menu(world, canvas, zox_id(MenuMain));
    if (!main_menu || zox_tst_menu(world, canvas, zox_id(MenuOptions))) {
        return 0;
    }
    if (!zox_tst_menus_click(world, player, main_menu, &button_event_menu_options, "options")) {
        return 1;
    }
    zox_tst_leaks_cycle++;
    zox_tst_menus_stage = 1;
    zox_tst_menus_frames = 0;
    return 0;
}

void zox_tst_ui_leaks_check(ecs* world) {
    (void) world;
}
