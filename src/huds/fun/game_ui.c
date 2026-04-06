// spawn our games canvas
entity spawn_game_canvas(ecs *world, entity ui_camera, int2 dimensions, float4 screen_to_canvas, entity app) {

    entity canvas = spawn_canvas(world, prefab_canvas, ui_camera, dimensions, screen_to_canvas, app);
    spawn_canvas_overlay(world, prefab_canvas_overlay, canvas, dimensions);

    // Tooltip on player
    spawn_tooltip(world, prefab_tooltip, canvas);

    // custom cursor
    entity mouse_pointer = 0;
    if (local_mouse) {
        zox_geter(local_mouse, Children, zevices);

        mouse_pointer = zevices->value[0];
    }

    // SDL_ShowCursor(SDL_DISABLE);
    entity texture_mouse = string_hashmap_get(files_hashmap_textures, new_string_data("cursor_01"));
    if (!texture_mouse) {
        zox_log_error("[cursor_01] mouse texture not found");
    }

    byte mouse_ui_size = 6 * ui_scale;
    entity prefab_mouse = prefab_element_shell;

    entity e = spawn_icon_mouse_follow_canvas(world, prefab_mouse, canvas, dimensions, max_layers2D - 2, float2_zero, mouse_ui_size, mouse_pointer);
    zox_set_unique_name(e, "mouse_ui");

    zox_add_tag(e, MouseElement);

    zox_set(e, MeshAlignment, { zox_mesh_alignment_top_left });
    clone_texture_data_scale(world, e, texture_mouse, int2_single(mouse_ui_size));

    zox_set(e, LayoutSize, { int2_single(mouse_ui_size) });
    zox_set(e, LayoutSizeDirty, { zox_dirty_trigger });
    zox_set(e, LayoutPositionDirty, { zox_dirty_trigger });

    spawn_gpu_texture(world, e);

    if (local_mouse) {
        zox_set(local_mouse, TextureLink, { e });
    }

    // testing from prefab
    // zox_set(e, RenderDisabled, { 0 });
    // zox_remove(e, GenerateTexture);

    /*zox_add_tag(e, Icon);
    zox_add_tag(e, IconTexture);
    zox_prefab_set(e, IconType, { 0 });
    zox_set(e, IconRadius, { default_icon_radius });

    zox_set(e, RenderDisabled, { 1 });*/

    // ### Mouse Pickup UI ###
    int icon_size = (default_icon_size / 4) * ui_scale;

    icon_mouse_follow = spawn_icon_mouse_follow_canvas(world, prefab_icon_mouse_follow, canvas, dimensions, max_layers2D - 3, float2_half, icon_size, mouse_pointer);
    zox_set_unique_name(icon_mouse_follow, "icon_mouse");

    return canvas;
}

// TODO: Refactor to Cameras (camera spawning) + UIs (canvas spawning)
void spawn_all_players_cameras_canvases(ecs *world, int players_playing, entity app) {

    if (!app) {
        zox_logv("No spawning Cameras and Canvases.");
        return;
    }

    set_main_cameras((int) players_playing);
    float3 camera_position = float3_zero;
    float4 camera_rotation = quaternion_identity;
    zox_geter_value(app, WindowSize, int2, screen_size)
    CameraLinks cameras = { 0 };

    for (int i = 0; i < players_playing; i++) {
        entity player = zox_players[i];

        set_camera_transform_to_main_menu(&camera_position, &camera_rotation, terrain_depth);

        float4 screen_to_canvas = (float4) { 1 / (float) players_playing, 1, i / (float) players_playing, 0 };
        int2 viewport_size = screen_to_canvas_size(screen_size, screen_to_canvas);
        int2 viewport_position = screen_to_canvas_position(screen_size, screen_to_canvas);
        int2 svp_size = scale_viewport(viewport_size);

        entity2 spawned_cameras = spawn_camera_player(world, player, zox_game_camera_mode, camera_position, camera_rotation, screen_to_canvas, viewport_position, svp_size, viewport_size);

        entity game_camera = spawned_cameras.x;
        add_to_CameraLinks(&cameras, game_camera);
        add_to_CameraLinks(&cameras, spawned_cameras.y);
        set_camera_mode(world, game_camera, zox_game_camera_mode);

        entity canvas = spawn_game_canvas(world, spawned_cameras.y, viewport_size, screen_to_canvas, app);

        zox_set(player, CanvasLink, { canvas });
        zox_set(canvas, PlayerLink, { player });

        // spawns a render texture ui and links to camera
        create_camera_rbo_and_fbo(world, game_camera, svp_size);
        spawn_render_texture(world, prefab_render_texture, canvas, viewport_size, svp_size, game_camera);

        // remove these soon
        zox_canvases[i] = canvas;
        main_cameras[i] = game_camera;
        ui_cameras[i] = spawned_cameras.y;

        spawn_skybox(world, shader_skybox, game_camera);
        set_skybox_colors(world, menu_sky_color, menu_sky_bottom_color);
    }
    zox_set_ptr(app, CameraLinks, cameras);
}

extern void spawn_all_players_start_ui(ecs*);

void on_boot_game_ui(ecs* world, entity app) {
    // move to game ui??
    spawn_all_players_cameras_canvases(world, players_playing, app);
    spawn_all_players_start_ui(world);
}
