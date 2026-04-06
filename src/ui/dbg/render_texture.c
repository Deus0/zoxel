entity dbg_render_texture = 0;
entity dbg_render_camera = 0;
entity dbg_render_cube= 0;

extern entity spawn_window_inspector(ecs*, entity, entity, entity);

// TODO: render texture shouldn't take up entire screen - just ui, mesh issue??
// TODO:

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

        entity p = prefab_element_textured;
        p = prefab_element_shell;
        // p = prefab_render_texture;

        int2 position = int2_single(256);
        int2 size = int2_single(333);
        int2 tsize = size; // int2_single(333);
        float2 anchor = float2_half;
        entity parent = canvas;
        byte layer = 2;
        color fcolor = color_white;
        color ocolor = color_white;

        int2 vp_position = int2_zero;
        zox_geter_value(canvas, LayoutSize, int2, vp_size);
        vp_size = size;

        dbg_render_camera = spawn_camera(world, prefab_camera, cposition, crotation, 0, 45, vp_position, vp_size, single_screen_to_canvas);
        create_camera_rbo_and_fbo(world, dbg_render_camera, vp_size);

        // dbg_render_texture = spawn_render_texture(world, prefab_render_texture, canvas, vp_size, vp_size, dbg_render_camera);
        dbg_render_texture = spawn_element2(world, p, canvas, parent, position, size, tsize, anchor, layer, fcolor, ocolor);
        zox_set(dbg_render_texture, CameraLink, { dbg_render_camera });

        zox_set_unique_name(dbg_render_texture, "dbg_render_texture");
        zox_add_tag(dbg_render_texture, RenderTexture);
        // for now we must do this otherwise we get edge pixels
        // zox_set(e, AnchorSize, { float2_one });
        float scale1 = 1;
        zox_set(dbg_render_texture, Scale1D, { scale1 });
        zox_set(dbg_render_texture, TransformMatrix, {
            float4x4_transform_scale(float3_zero, quaternion_identity, scale1)
        });

        // our scene
        float3 cp = float3_add(cposition,
            quaternion_rotate_vector(crotation, float3_backward));
        dbg_render_cube = spawn_cube(world, prefab_cube, cp);

        zox_log("Spawned [dbg_render_texture]");
        spawn_sound_from_file_index(world, prefab_sound, 0);

        // spawn_window_inspector(world, canvas, player, dbg_render_texture);
    }
}
