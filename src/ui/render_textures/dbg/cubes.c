entity dbg_render_texture = 0;
entity dbg_render_camera = 0;
entity dbg_render_cube= 0;

void zox_tst_render_texture(ecs *world, ClickEventData data) {
    byte is_camera_filtering = 1;
    float downscale = 1;
    float cube_scale = 0.2f;
    float orbit_range = 2;
    byte layer = max_layers2D - 10;
    color clear = (color) { 0, 55, 55, 0 };
    float3 spawn_position = (float3) { 0, 0, -1.5f };
    if (dbg_render_texture) {
        zox_log("Deleting [dbg_render_texture]");
        zox_delete(dbg_render_texture);
        zox_delete(dbg_render_camera);
        zox_delete(dbg_render_cube);
        dbg_render_texture = 0;
        dbg_render_camera = 0;
        dbg_render_cube = 0;
        spawn_sound_from_file_index(world, prefab_sound, 1);
        return;
    }
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    entity canvas = get_linked_canvas(world, player);
    entity camera = zox_get_link(world, player, CameraLink);
    float3 cposition = zox_getv(camera, Position3D);
    float4 crotation = zox_getv(camera, Rotation3D);
    int padding = 40;
    int2 lsize = int2_single(256);
    int2 tsize = int2_scale1(lsize, 1 / downscale);
    int2 position = (int2) { -lsize.x / 2, -lsize.y / 2 };
    position.x -= padding;
    position.y -= padding;
    float2 anchor = (float2) { 1, 1 };
    // TODO: spawn_render_camera instead
    entity render_camera = spawn_camera(
        world,
        prefab_camera3,
        cposition,
        crotation,
        0, 45,
        int2_zero,
        tsize,
        single_screen_to_canvas);
    zox_set_unique_name(render_camera, "dbg_render_texture_camera");
    zox_add(render_camera, RenderCamera);
    zox_setv(render_camera, Color, clear);
    zox_setv(render_camera, CameraBlur, 0);
    zox_setv(render_camera, CameraVignette, 2.5f);
    if (is_camera_filtering) {
        zox_add(render_camera, CameraFilter);
    }
    dbg_render_camera = render_camera;
    entity material = spawn_material_render_texture(world, 1);
    // Create Render Texture
    entity ui = spawn_render_texture(
        world,
        prefab_render_texture,
        canvas,
        anchor,
        position,
        lsize,
        tsize,
        layer,
        camera,
        material);
    zox_set_unique_name(ui, "dbg_render_texture");
    zox_set_parent(world, material, ui);
    zox_add(ui, RenderTextureAlpha);
    zox_setv(ui, Alpha, 0.8f);
    zox_link(world, ui, MaterialLink, material);
    zox_link(world, camera, MaterialLink, material);
    dbg_render_texture = ui;
    // our scene
    {
        float3 cube_position = float3_add(cposition, quaternion_rotate_vector(crotation, spawn_position));
        entity cube = spawn_cube(
            world,
            prefab_cube,
            cube_position,
            cube_scale);
        zox_set_unique_name(cube, "dbg_render_texture_cube");
        add_eternal_euler(world, cube, (float3) { 24, 24, 0 });
        zox_setv(cube, Color, color_green);
        if (is_camera_filtering) {
            zox_set(cube, CameraRenderer, { camera });
        }
        dbg_render_cube = cube;
        {
            entity cube = spawn_cube(
                world,
                prefab_cube,
                float3_zero,
                0.09f);
            add_eternal_euler(world, cube, (float3) { -4, -12, 0 });
            zox_set(cube, LocalPosition3D, {{ orbit_range, 0, 0 }});
            zox_setv(cube, LocalScale1, 0.5f);
            zox_set_parent(world, cube, dbg_render_cube);
            zox_setv(cube, Color, color_blue);
            if (is_camera_filtering) {
                zox_set(cube, CameraRenderer, { camera });
            }
        }
        {
            entity cube = spawn_cube(
                world,
                prefab_cube,
                float3_zero,
                0.1f);
            add_eternal_euler(world, cube, (float3) { 4, 16, 0 });
            zox_set(cube, LocalPosition3D, {{ 0, orbit_range, 0 }});
            zox_setv(cube, LocalScale1, 0.3f);
            zox_set_parent(world, cube, dbg_render_cube);
            zox_setv(cube, Color, color_red);
            if (is_camera_filtering) {
                zox_set(cube, CameraRenderer, { camera });
            }
        }
    }
    // zox_log("Spawned [dbg_render_texture]");
    spawn_sound_from_file_index(world, prefab_sound, 0);
    // spawn_inspector(world, canvas, player, dbg_render_texture);
    // spawn_inspector(world, canvas, player, dbg_render_cube);
}
