// TODO: Refactor to Cameras (camera spawning) + UIs (canvas spawning)
extern entity spawn_menu_start(ecs*, entity, entity);

// NOTE: Runs on boot, spawns game cameras, uis
void on_boot_game_ui(
    ecs *world,
    entity app)
{
    byte dbg_log = 0;
    // Headless has none
    if (!zox_valid(app) || !zox_has(app, WindowSize)) {
        zox_logv("No spawning Cameras and Canvases");
        return;
    }
    set_main_cameras(players_playing);
    float3 camera_position = main_menu_camera_position;
    float4 camera_rotation = main_menu_camera_rotation;
    int2 screen_size = zox_getv(app, WindowSize);
    for (int i = 0; i < players_playing; i++) {
        // TODO: Use player children of app
        entity player = zox_players[i];
        if (dbg_log) {
            zox_log("Spawning App [%s] Player [%s] Game UI",
                zox_getn(app),
                zox_getn(player));
        }
        float4 screen_to_canvas = (float4) { 1 / (float) players_playing, 1, i / (float) players_playing, 0 };
        int2 vp_size = screen_to_canvas_size(screen_size, screen_to_canvas);
        int2 viewport_position = screen_to_canvas_position(screen_size, screen_to_canvas);
        int2 svp_size = scale_viewport(vp_size);
        // hmmm
#ifndef zox_xr
        entity2 spawned_cameras = spawn_player_cameras(
            world,
            app,
            player,
            zox_game_camera_mode,
            camera_position,
            camera_rotation,
            screen_to_canvas,
            viewport_position,
            svp_size,
            vp_size);
        entity game_camera = spawned_cameras.x;
        entity ui_camera = spawned_cameras.y;
        // remove these soon
        main_cameras[i] = game_camera;
        ui_cameras[i] = spawned_cameras.y;
        set_camera_mode(
            world,
            game_camera,
            zox_game_camera_mode);
#else
        entity ui_camera = 0;
#endif
        entity canvas = spawn_canvas(
            world,
            prefab_canvas,
            ui_camera,
            vp_size,
            screen_to_canvas,
            app);
        zox_set_unique_name(canvas, "game_canvas");
        zox_setv(player, CanvasLink, canvas);
        zox_setv(canvas, PlayerLink, player);
        entity mouse_pointer = 0;
        if (zox_valid(local_mouse)) {
            mouse_pointer = zox_get_child_by_id(
                world,
                local_mouse,
                zox_id(ZevicePointer));
            if (!zox_valid(mouse_pointer)) {
                zox_logw("Mouse Pointer Invalid on Mouse [%s]", zox_getn(local_mouse));
            }
        } else {
            zox_logw("Mouse Invalid on Player [%s]", zox_getn(player));
        }
        // Tooltip on player
        spawn_tooltip(
            world,
            prefab_tooltip,
            canvas,
            local_mouse,
            mouse_pointer);
        // custom cursor
        // SDL_ShowCursor(SDL_DISABLE);
        entity texture_mouse = string_hashmap_get(files_hashmap_textures, new_string_data("cursor"));
        if (!texture_mouse) {
            zox_log_error("[cursor] mouse texture not found");
        }
        byte mouse_ui_size = 8 * ui_scale;
        entity mouse_ui = spawn_mouse_icon(
            world,
            prefab_element,
            canvas,
            max_layers2D - 2,
            float2_zero,
            mouse_ui_size,
            local_mouse,
            mouse_pointer);
        zox_set_unique_name(mouse_ui, "mouse_ui");
        zox_add(mouse_ui, MouseElement);
        zox_setv(mouse_ui, MeshAlignment, zox_alignment_top_left);
        clone_texture_data_scale(
            world,
            mouse_ui,
            texture_mouse,
            int2_single(mouse_ui_size));
        zox_set(mouse_ui, LayoutSize, { int2_single(mouse_ui_size) });
        zox_set(mouse_ui, LayoutSizeDirty, { zox_dirty_trigger });
        zox_set(mouse_ui, LayoutPositionDirty, { zox_dirty_trigger });
        if (local_mouse) {
            zox_setv(local_mouse, TextureLink, mouse_ui);
        } else {
            zox_loge("No Local Mouse for our UI");
        }
        // Mouse Pickup UI
        int icon_size = (default_icon_size / 4) * ui_scale;
        entity e2 = spawn_mouse_icon(
            world,
            prefab_element,
            canvas,
            max_layers2D - 3,
            float2_half,
            icon_size,
            local_mouse,
            mouse_pointer);
        zox_set_unique_name(e2, "data_mouse");
        zox_add(e2, DataMouse);
        entity empty_texture = string_hashmap_get(files_hashmap_textures, new_string_data("empty"));
        clone_texture_data(world, e2, empty_texture);
        if (!zox_disable_screen_fader) {
            spawn_canvas_overlay(
                world,
                prefab_canvas_overlay,
                canvas,
                vp_size);
        }
        // main menu ui
        spawn_menu_start(
            world,
            player,
            canvas);
#ifndef zox_xr
        // spawns a render texture ui and links to camera
        if (!zox_disable_post_processing) {
            // material_render_texture_rgba
            entity material = zox_game_render_texture_alpha ?
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
            if (!zox_game_render_texture_alpha) {
                zox_add(texture, TextureRGB);
            }
        }
#endif
    }
}

/*void on_boot_game_ui(ecs* world, entity app) {
    spawn_all_players_cameras_canvases(
        world,
        players_playing,
        app);
    spawn_all_players_start_ui(world);
}*/
