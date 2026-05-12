entity dbg_render_texture = 0;
entity dbg_render_camera = 0;
entity dbg_render_cube= 0;
extern entity spawn_inspector(ecs*, entity, entity, entity);
// TODO: render texture shouldn't take up entire screen - just ui, mesh issue??

void zox_tst_render_texture(ecs *world, ClickEventData data) {
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
    zox_geter_value(player, CameraLink, entity, camera);
    zox_geter_value(camera, Position3D, float3, cposition);
    zox_geter_value(camera, Rotation3D, float4, crotation);
    entity prefab = prefab_render_texture;
    byte layer = 2;
    int padding = 40;
    float downscale = 1;
    int2 lsize = int2_single(256);
    int2 tsize = int2_scalef(lsize, 1 / downscale);
    int2 position = (int2) { -lsize.x / 2, -lsize.y / 2 };
    position.x -= padding;
    position.y -= padding;
    float2 anchor = (float2) { 1, 1 };
    // entity parent = canvas;
    // color fcolor = color_white;
    // color ocolor = color_white;
    dbg_render_camera = spawn_camera(world, prefab_camera, cposition, crotation, 0, 45, int2_zero, tsize, single_screen_to_canvas);
    zox_add_tag(dbg_render_camera, CameraFilter);
    create_camera_rbo_and_fbo(world, dbg_render_camera, tsize);
    // Create texture
    dbg_render_texture = spawn_ui(world, prefab, canvas, anchor, position, lsize, tsize);
    zox_set(dbg_render_texture, Layer2D, { layer });
    zox_set(dbg_render_texture, CameraLink, { dbg_render_camera });
    zox_set_unique_name(dbg_render_texture, "dbg_render_texture");
    // our scene
    float3 cube_position = float3_add(cposition, quaternion_rotate_vector(crotation, (float3) { 0, 0, -1.5f }));
    dbg_render_cube = spawn_cube(world, prefab_cube, cube_position, 0.16f);
    zox_set(dbg_render_cube, CameraRenderer, { dbg_render_camera });
    add_eternal_euler(world, dbg_render_cube, (float3) { 24, 24, 0 });
    entity e2 = spawn_cube(world, prefab_cube, float3_zero, 0.09f);
    zox_set(e2, CameraRenderer, { dbg_render_camera });
    add_eternal_euler(world, e2, (float3) { -4, -12, 0 });
    zox_set(e2, LocalPosition3D, {{ 0.5f, 0, 0 }});
    // zox_set(e2, ParentLink, { dbg_render_cube });
    zox_set_parent(world, e2, dbg_render_cube);
    entity e3 = spawn_cube(world, prefab_cube, float3_zero, 0.1f);
    zox_set(e3, CameraRenderer, { dbg_render_camera });
    add_eternal_euler(world, e3, (float3) { 4, 16, 0 });
    zox_set(e3, LocalPosition3D, {{ 0, 0.5f, 0 }});
    // zox_set(e3, ParentLink, { dbg_render_cube });
    zox_set_parent(world, e3, dbg_render_cube);
    // zox_log("Spawned [dbg_render_texture]");
    spawn_sound_from_file_index(world, prefab_sound, 0);
    // spawn_inspector(world, canvas, player, dbg_render_texture);
    // spawn_inspector(world, canvas, player, dbg_render_cube);
}
