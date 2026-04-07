entity dbg_render_texture = 0;
entity dbg_render_camera = 0;
entity dbg_render_cube= 0;

extern entity spawn_window_inspector(ecs*, entity, entity, entity);

// TODO: render texture shouldn't take up entire screen - just ui, mesh issue??

void spawn_test_render_texture(ecs *world, int32_t keycode) {

    if (keycode != SDLK_0) {
        return;
    }

    if (dbg_render_texture) {

        zox_log("Deleting [dbg_render_texture]");

        zox_delete(dbg_render_texture);
        zox_delete(dbg_render_camera);
        zox_delete(dbg_render_cube);

        dbg_render_texture = 0;
        dbg_render_camera = 0;
        dbg_render_cube = 0;

        spawn_sound_from_file_index(world, prefab_sound, 1);

    } else {

        entity player = dbg_player;
        if (!zox_valid(player)) {
            return;
        }

        entity canvas = get_linked_canvas(world, player);

        zox_geter_value(player, CameraLink, entity, camera);
        zox_geter_value(camera, Position3D, float3, cposition);
        zox_geter_value(camera, Rotation3D, float4, crotation);

        entity p = prefab_render_texture;
        byte layer = 2;
        int padding = 40;
        float downscale = 4;
        int2 lsize = int2_single(256);
        int2 tsize = int2_scalef(lsize, 1 / downscale); // lsize; // int2_single(333);
        int2 position = (int2) { -lsize.x / 2, -lsize.y / 2 };
        position.x -= padding;
        position.y -= padding;
        float2 anchor = (float2) { 1, 1 }; // float2_half;
        entity parent = canvas;
        color fcolor = color_white;
        color ocolor = color_white;

        dbg_render_camera = spawn_camera(world, prefab_camera, cposition, crotation, 0, 45, int2_zero, tsize, single_screen_to_canvas);
        zox_add_tag(dbg_render_camera, CameraFilter);
        create_camera_rbo_and_fbo(world, dbg_render_camera, tsize);

        // Create texture
        dbg_render_texture = spawn_element2(world, p, canvas, parent, position, lsize, tsize, anchor, layer, fcolor, ocolor);
        zox_set(dbg_render_texture, CameraLink, { dbg_render_camera });
        zox_set_unique_name(dbg_render_texture, "dbg_render_texture");

        // our scene
        float3 cube_position = float3_add(cposition, quaternion_rotate_vector(crotation, float3_scale(float3_backward, 0.3f)));
        // float3 cube_position = cposition;
        dbg_render_cube = spawn_cube(world, prefab_cube, cube_position);
        zox_set(dbg_render_cube, CameraRenderer, { dbg_render_camera });
        float4 rotation_speed = quaternion_from_euler( (float3) { 0, 25 * degreesToRadians, 0 });
        zox_set(dbg_render_cube, EternalRotation, { rotation_speed });

        // zox_log("Spawned [dbg_render_texture]");
        spawn_sound_from_file_index(world, prefab_sound, 0);

        // spawn_window_inspector(world, canvas, player, dbg_render_texture);
    }
}
