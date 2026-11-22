// TODO: Refactor to Cameras (camera spawning) + UIs (canvas spawning)
extern entity spawn_menu_start(ecs*, entity, entity);
extern void autostart_new_realm(ecs*, entity);

// spawns a player's screen: its game and ui cameras, canvas, mouse, fader and render texture
//  - index of count players, side by side
//  - the first player's at boot (with the start menu), the others' as a game starts (split screen, game_state_split_screen)
void spawn_player_screen(
    ecs *world,
    entity app,
    entity player,
    int i,
    int count,
    byte is_boot)
{
    byte dbg_log = 0;
    float3 camera_position = main_menu_camera_position;
    float4 camera_rotation = main_menu_camera_rotation;
    int2 screen_size = zox_getv(app, WindowSize);
    {
        if (dbg_log) {
            zox_log("Spawning App [%s] Player [%s] Game UI",
                zox_getn(app),
                zox_getn(player));
        }
        // split screen: side by side, one column each
        const float4 screen_to_canvas = viewport_grid_cell(i, count, 1);
        int2 vp_size = screen_to_canvas_size(screen_size, screen_to_canvas);
        int2 viewport_position = screen_to_canvas_position(screen_size, screen_to_canvas);
        int2 svp_size = scale_viewport(vp_size);
        // hmmm
        // XR: its own camera renders each eye (xr/ins), no player or ui cameras
        entity game_camera = 0;
        entity ui_camera = 0;
        if (!zox_xr_active) {
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
            game_camera = spawned_cameras.x;
            ui_camera = spawned_cameras.y;
            // remove these soon
            main_cameras[i] = game_camera;
            // hears 3D sounds (split screen: the first player's)
            if (i == 0) {
                zox_add(game_camera, SoundListener);
            }
            ui_cameras[i] = spawned_cameras.y;
            set_camera_mode(
                world,
                game_camera,
                zox_game_camera_mode);
        }
        entity canvas = spawn_canvas(
            world,
            prefab_canvas,
            ui_camera,
            vp_size,
            screen_to_canvas,
            app);
        zox_set_unique_name(canvas, "game_canvas");
        // the app's canvas is the first player's
        if (i == 0) {
            zox_link(world, app, CanvasLink, canvas);
        }
        zox_link(world, player, CanvasLink, canvas);
        zox_link(world, canvas, PlayerLink, player);
        zox_link(world, canvas, Player, player);
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
        // the mouse's cursor and held icon: the first player's, the keyboard and mouse stay with them
        if (i == 0) {
            // custom cursor
            // SDL_ShowCursor(SDL_DISABLE);
            entity texture_mouse = string_hashmap_get(files_hashmap_textures, new_string_data("cursor"));
            if (!texture_mouse) {
                zox_loge("[cursor] mouse texture not found");
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
            zox_add(mouse_ui, LayoutSizeDirty);
            if (local_mouse) {
                zox_link(world, local_mouse, TextureLink, mouse_ui);
                // zox_setv(local_mouse, TextureLink, mouse_ui);
            } else {
                zox_loge("No Local Mouse for our UI");
            }
        }
        // the held item (picked up from a slot): every player's, on the mouse for the first player's mouse, else
        //  top right (data_mouse_corner_system: a navigator has no pointer)
        {
            int icon_size = (default_icon_size / 4) * ui_scale;
            entity e2 = spawn_mouse_icon(
                world,
                prefab_element,
                canvas,
                max_layers2D - 3,
                float2_half,
                icon_size,
                i == 0 ? local_mouse : 0,
                i == 0 ? mouse_pointer : 0);
            zox_set_unique_name(e2, "data_mouse");
            zox_add(e2, DataMouse);
            entity empty_texture = string_hashmap_get(files_hashmap_textures, new_string_data("empty"));
            clone_texture_data(world, e2, empty_texture);
        }
        if (!zox_disable_screen_fader) {
            spawn_canvas_overlay(
                world,
                prefab_canvas_overlay,
                canvas,
                vp_size);
        }
        // main menu ui
        // NOTE: tests that check the menus (zox_tst_menus) ignore --autostart
        if (!is_boot) {
            // in game: its game ui spawns with its player state
        } else if (zox_autostart && !zox_tst_menus) {
            autostart_new_realm(world, player);
        } else {
            spawn_menu_start(
                world,
                player,
                canvas);
        }
        // spawns a render texture ui and links to camera
        if (!zox_xr_active && !zox_disable_post_processing) {
            // each screen its own material: the camera's vignette, blur, tint and lids are uniforms on it (one shared,
            //  a player in dialogue or asleep darkened every screen); the first's lives on, the others' go with it
            entity material = zox_game_render_texture_alpha ?
                material_render_texture_rgba :
                material_render_texture_rgb;
            if (i > 0) {
                const entity shader = zox_get_link(world, material, ShaderLink);
                material = spawn_material_render_texture_from(world, shader, zox_getv(shader, ShaderGPULink));
            }
            zox_link(world, game_camera, MaterialLink, material);
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
            if (i > 0) {
                zox_set_parent(world, material, texture);
            }
        }
    }
}

// NOTE: Runs on boot, spawns the first player's cameras and ui (more players split the screen in game)
void on_boot_game_ui(
    ecs *world,
    entity app)
{
    // Headless has none
    if (!zox_valid(app) || !zox_has(app, WindowSize)) {
        zox_logv("No spawning Cameras and Canvases");
        return;
    }
    set_main_cameras(1);
    spawn_player_screen(world, app, zox_players[0], 0, 1, 1);
}
