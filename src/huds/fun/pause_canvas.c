extern entity spawn_menu_paused(ecs*, entity);
// the pause menu: one over the whole window for every player (in split screen too), on a canvas no player owns
//  (shared_canvas, with its own ui camera), spawned as the game pauses and gone as it plays; any player's gamepad
//  navigates it (element_navigation_begin_system), any click resumes or leaves for all
//  - each player linked to it too (CanvasLink, many): their navigators and mouse reach it; their own canvas is the one
//    whose PlayerLink is them
entity shared_canvas = 0;
static entity shared_canvas_camera = 0;

void despawn_shared_canvas(ecs* world) {
    for (int i = 0; i < players_playing; i++) {
        if (zox_valid(shared_canvas) && zox_valid(zox_players[i])) {
            zox_unlink(world, zox_players[i], CanvasLink, shared_canvas);
        }
    }
    if (zox_valid(shared_canvas_camera)) {
        if (zox_valid(main_app)) {
            zox_unlink(world, main_app, ViewportCamera, shared_canvas_camera);
        }
        zox_delete(shared_canvas_camera);
    }
    if (zox_valid(shared_canvas)) {
        zox_delete(shared_canvas);
    }
    shared_canvas_camera = 0;
    shared_canvas = 0;
}

entity spawn_shared_canvas(ecs* world) {
    const entity app = main_app;
    if (zox_valid(shared_canvas) || !zox_valid(app) || !zox_has(app, WindowSize)) {
        return shared_canvas;
    }
    const int2 size = zox_getv(app, WindowSize);
    const entity camera = spawn_camera_ui(world, prefab_camera_ui, int2_zero, size, single_screen_to_canvas);
    zox_set_parent(world, camera, app);
    zox_link(world, app, ViewportCamera, camera);   // sized with the window (viewport_resize_system)
    zox_set_unique_name(camera, "camera_shared_ui");
    const entity canvas = spawn_canvas(world, prefab_canvas, camera, size, single_screen_to_canvas, app);
    zox_set_unique_name(canvas, "shared_canvas");
    // every player's: any of them navigates it, clicks it
    for (int i = 0; i < players_playing; i++) {
        if (zox_valid(zox_players[i])) {
            zox_link(world, zox_players[i], CanvasLink, canvas);
        }
    }
    shared_canvas_camera = camera;
    shared_canvas = canvas;
    return canvas;
}

// the canvas the pause menu goes on: the shared one; 3D huds (a headset, one player) its player's, a 3D window is
//  placed for its canvas's player
static inline entity pause_menu_canvas(ecs* world) {
    return zox_huds3D ? zox_get_link(world, zox_players[0], CanvasLink) : shared_canvas;
}

// the pause menu, where it is (pause_menu_canvas), 0 none
entity get_pause_menu(ecs* world) {
    const entity canvas = pause_menu_canvas(world);
    const entity e = zox_valid(canvas) ? zox_get_child_by_id(world, canvas, zox_id(MenuPaused)) : 0;
    return zox_valid(e) ? e : 0;
}

void delayed_spawn_menu_paused(ecs* world, entity game) {
    const entity canvas = pause_menu_canvas(world);
    if (!zox_valid(game) || zox_getv(game, GameState) != zox_game_state_paused || !zox_valid(canvas) ||
        zox_valid(zox_get_child_by_id(world, canvas, zox_id(MenuPaused)))) {
        return;
    }
    spawn_menu_paused(world, canvas);
    // every player's navigators select in it (one in its own game menus was kept there)
    for (int i = 0; i < players_playing; i++) {
        entity devices[zox_children_capacity];
        const uint length = zox_get_children_by_id(world, zox_players[i], devices, zox_children_capacity, zox_id(DeviceNavigator));
        for (uint j = 0; j < length; j++) {
            raycaster_select_element(world, devices[j], 0);
        }
    }
}

// as the game pauses, the pause menu (a frame later, its canvas spawned); as it plays or ends, gone
void game_state_pause_menu(ecs* world, entity game, byte state) {
    if (state == zox_game_state_paused) {
        if (!zox_huds3D) {
            spawn_shared_canvas(world);
        }
        delay_event(world, &delayed_spawn_menu_paused, game, 0.01f);
    } else if (zox_valid(shared_canvas)) {
        despawn_shared_canvas(world);
    }
}
