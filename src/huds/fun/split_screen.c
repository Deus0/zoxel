// local coop split screen: the first player's screen fills the window on the menus, in game each player has
//  their own, side by side (one column each, viewport_grid_cell)
//  - joined players (join_player) get their cameras and canvas as a game loads, and lose them back at the main menu

// a split screen's uis are scaled to its width: a half screen's are half, laid out as a full screen's are, and no
//  bigger than fits split_screen_hud_width across it (they overlapped, the stats over the minimap)
float player_ui_scale_saved = 0;
// the top huds side by side at ui scale 1, with slack: the stats panel (~111) and the minimap (80)
#define split_screen_hud_width 220

static inline float get_canvas_ui_fraction(ecs* world, entity canvas) {
    if (!zox_valid(canvas) || !zox_has(canvas, ScreenToCanvas)) {
        return 1;
    }
    const float4 cell = zox_getv(canvas, ScreenToCanvas);
    const float width = cell.x;    // its width of the window's (viewport_grid_cell)
    return width > 0 && width < 1 ? width : 1;
}

void player_ui_scale_begin(ecs* world, entity player) {
    player_ui_scale_saved = ui_scale;
    const entity canvas = zox_get_link(world, player, CanvasLink);
    const float fraction = get_canvas_ui_fraction(world, canvas);
    if (fraction >= 1) {
        return;
    }
    ui_scale *= fraction;
    // its canvas's width (LayoutSize is a frame behind a split, the window's share is now)
    if (zox_valid(main_app) && zox_has(main_app, WindowSize)) {
        const float width = zox_getv(main_app, WindowSize).x * fraction;
        const float fits = width / split_screen_hud_width;
        if (ui_scale > fits) {
            ui_scale = fits;
        }
    }
}

void player_ui_scale_end(ecs* world, entity player) {
    (void) world;
    (void) player;
    ui_scale = player_ui_scale_saved;
}

// for a player state observer (core Dirty): calls fun on each player, its uis scaled to its split screen
void player_state_each(iter* it, void (*fun)(ecs*, entity, byte)) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        player_ui_scale_begin(world, e);
        fun(world, e, state->value);
        player_ui_scale_end(world, e);
    }
}

// places each player's cameras and canvas in their cell of the window
void layout_player_screens(
    ecs* world,
    entity app,
    int count)
{
    for (int i = 0; i < count; i++) {
        const entity player = zox_players[i];
        const float4 cell = viewport_grid_cell(i, count, 1);
        const entity camera = zox_get_link(world, player, CameraLink);
        const entity canvas = zox_get_link(world, player, CanvasLink);
        const entity ui_camera = zox_valid(canvas) ? zox_get_link(world, canvas, CameraLink) : 0;
        if (zox_valid(camera)) {
            zox_setv(camera, ScreenToCanvas, cell);
        }
        if (zox_valid(ui_camera)) {
            zox_setv(ui_camera, ScreenToCanvas, cell);
        }
        if (zox_valid(canvas)) {
            zox_setv(canvas, ScreenToCanvas, cell);
            zox_add(canvas, Dirty);
        }
        // a screen spawned this frame set its own (its links are deferred)
        if (zox_valid(camera)) {
            main_cameras[i] = camera;
        }
        if (zox_valid(ui_camera)) {
            ui_cameras[i] = ui_camera;
        }
    }
    for (int i = count; i < max_cameras; i++) {
        main_cameras[i] = 0;
        ui_cameras[i] = 0;
    }
    set_main_cameras(count);
    // the viewports follow (viewport_resize_system), canvases resize themselves (canvas_resize_system)
    if (zox_valid(app)) {
        zox_add(app, WindowSizeDirty);
    }
}

// a joined player's screen goes: its cameras and canvas
void despawn_player_screen(
    ecs* world,
    entity app,
    entity player)
{
    const entity camera = zox_get_link(world, player, CameraLink);
    const entity canvas = zox_get_link(world, player, CanvasLink);
    const entity ui_camera = zox_valid(canvas) ? zox_get_link(world, canvas, CameraLink) : 0;
    if (zox_valid(camera)) {
        zox_unlink(world, app, ViewportCamera, camera);
        zox_unlink(world, player, CameraLink, camera);
        zox_delete(camera);
    }
    if (zox_valid(ui_camera)) {
        zox_unlink(world, app, ViewportCamera, ui_camera);
        zox_delete(ui_camera);
    }
    if (zox_valid(canvas)) {
        zox_unlink(world, player, CanvasLink, canvas);
        zox_delete(canvas);
    }
}

// as a game loads, joined players get their screens, and the window splits
void game_state_split_screen(
    ecs* world,
    entity game,
    byte state)
{
    (void) game;
    if (state != zox_game_load_start || players_playing < 2 || zox_xr_active) {
        return;
    }
    const entity app = main_app;
    if (!zox_valid(app) || !zox_has(app, WindowSize)) {
        return;
    }
    for (int i = 1; i < players_playing; i++) {
        if (!zox_valid(zox_get_link(world, zox_players[i], CanvasLink))) {
            spawn_player_screen(world, app, zox_players[i], i, players_playing);
        }
    }
    layout_player_screens(world, app, players_playing);
}

// back at the main menu: the first player's screen fills the window again
void end_split_screen(ecs* world) {
    const entity app = main_app;
    if (!zox_valid(app) || main_cameras_count < 2) {
        return;
    }
    for (int i = 1; i < players_playing; i++) {
        despawn_player_screen(world, app, zox_players[i]);
    }
    layout_player_screens(world, app, 1);
}
