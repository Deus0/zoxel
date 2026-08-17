extern void spawn_all_players_start_ui(ecs*);

// Spawn our games canvas
// NOTE: Runs on boot
entity spawn_game_canvas(ecs *world, entity ui_camera, int2 dimensions, float4 screen_to_canvas, entity app) {
    entity canvas = spawn_canvas(world, prefab_canvas, ui_camera, dimensions, screen_to_canvas, app);
    zox_set_unique_name(canvas, "game_canvas");
    if (!zox_disable_screen_fader) {
        spawn_canvas_overlay(world, prefab_canvas_overlay, canvas, dimensions);
    }
    // Tooltip on player
    spawn_tooltip(world, prefab_tooltip, canvas);
    // custom cursor
    entity mouse_pointer = 0;
    if (local_mouse) {
        mouse_pointer = zox_get_child_by_id(world, local_mouse, zox_id(ZevicePointer));
    }
    // SDL_ShowCursor(SDL_DISABLE);
    entity texture_mouse = string_hashmap_get(files_hashmap_textures, new_string_data("cursor"));
    if (!texture_mouse) {
        zox_log_error("[cursor] mouse texture not found");
    }
    byte mouse_ui_size = 8 * ui_scale;
    entity e = spawn_mouse_icon(world, prefab_element, canvas, dimensions, max_layers2D - 2, float2_zero, mouse_ui_size, mouse_pointer);
    zox_set_unique_name(e, "mouse_ui");
    zox_add(e, MouseElement);
    zox_set(e, MeshAlignment, { zox_alignment_top_left });
    clone_texture_data_scale(world, e, texture_mouse, int2_single(mouse_ui_size));
    zox_set(e, LayoutSize, { int2_single(mouse_ui_size) });
    zox_set(e, LayoutSizeDirty, { zox_dirty_trigger });
    zox_set(e, LayoutPositionDirty, { zox_dirty_trigger });
    if (local_mouse) {
        zox_set(local_mouse, TextureLink, { e });
    }
    // Mouse Pickup UI
    int icon_size = (default_icon_size / 4) * ui_scale;
    entity e2 = spawn_mouse_icon(world, prefab_element, canvas, dimensions, max_layers2D - 3, float2_half, icon_size, mouse_pointer);
    zox_set_unique_name(e2, "data_mouse");
    zox_add(e2, DataMouse);
    entity empty_texture = string_hashmap_get(files_hashmap_textures, new_string_data("empty"));
    clone_texture_data(world, e2, empty_texture);
    return canvas;
}

// TODO: Refactor to Cameras (camera spawning) + UIs (canvas spawning)

// NOTE: Runs on boot
void spawn_all_players_cameras_canvases(
    ecs *world,
    int players_playing,
    entity app)
{
    // Headless has none
    if (!zox_valid(app) || !zox_has(app, WindowSize)) {
        zox_logv("No spawning Cameras and Canvases");
        return;
    }
    set_main_cameras((int) players_playing);
    float3 camera_position = main_menu_camera_position;
    float4 camera_rotation = main_menu_camera_rotation;
    int2 screen_size = zox_getv(app, WindowSize);
    for (int i = 0; i < players_playing; i++) {
        entity player = zox_players[i];
        float4 screen_to_canvas = (float4) { 1 / (float) players_playing, 1, i / (float) players_playing, 0 };
        int2 vp_size = screen_to_canvas_size(screen_size, screen_to_canvas);
        int2 viewport_position = screen_to_canvas_position(screen_size, screen_to_canvas);
        int2 svp_size = scale_viewport(vp_size);
        entity2 spawned_cameras = spawn_player_cameras(world, app, player, zox_game_camera_mode, camera_position, camera_rotation, screen_to_canvas, viewport_position, svp_size, vp_size);
        entity game_camera = spawned_cameras.x;
        set_camera_mode(world, game_camera, zox_game_camera_mode);
        entity canvas = spawn_game_canvas(world, spawned_cameras.y, vp_size, screen_to_canvas, app);
        zox_set(player, CanvasLink, { canvas });
        zox_set(canvas, PlayerLink, { player });
        // spawns a render texture ui and links to camera
        if (!zox_disable_post_processing) {
            // material_render_texture_rgba
            byte is_alpha = 1;
            entity material = is_alpha ?
                material_render_texture_rgba :
                material_render_texture_rgb;
            zox_setv(game_camera, MaterialLink, material);
            entity texture = spawn_render_texture(
                world,
                prefab_render_texture_screen,
                canvas,
                float2_centre,
                int2_zero,
                vp_size,
                svp_size,
                0,
                game_camera,
                material);
            if (!is_alpha) {
                zox_add(texture, TextureRGB);
            }
        }
        // remove these soon
        main_cameras[i] = game_camera;
        ui_cameras[i] = spawned_cameras.y;
    }
}

void on_boot_game_ui(ecs* world, entity app) {
    // move to game ui??
    spawn_all_players_cameras_canvases(world, players_playing, app);
    spawn_all_players_start_ui(world);
}
