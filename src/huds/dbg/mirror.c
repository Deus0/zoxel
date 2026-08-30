entity dbg_render_texture_character = 0;
entity dbg_render_texture_mesh = 0;
extern entity get_linked_character(ecs*, entity);

entity zox_tst_render_texture_character(
    ecs *world,
    entity player)
{
    float3 renderer_position = (float3) {
        randf_range(-100, 100),
        -100,
        randf_range(-100, 100),
    };
    float rotate_speed = 16;
    byte downscale = 4;
    float alpha = 0.9f;
    // byte is_camera_filtering = 1;
    // float downscale = 1;
    // byte layer = max_layers2D - 10;
    if (zox_valid(dbg_render_texture_character)) {
        zox_log("Deleting [dbg_render_texture_character]");
        zox_delete(dbg_render_texture_character);
        zox_delete(dbg_render_texture_mesh);
        dbg_render_texture_character = 0;
        return 0;
    }
    if (!zox_valid(player)) {
        return 0;
    }
    entity canvas = get_linked_canvas(world, player);
    entity character = get_linked_character(world, player);
    if (!zox_valid(character)) {
        zox_log("No character to render");
        return 0;
    }
    // int padding = 40;
    int2 size = int2_single(128 * ui_scale);
    int2 position = int2_zero;
    float2 anchor = float2_centre;
    // int2 tsize = int2_scale1(lsize, 1 / downscale);
    byte header_font_size = 8 * ui_scale;
    byte2 header_padding = (byte2) {
        10 * ui_scale,
        4 * ui_scale
    };
    entity mesh_clone =
        spawn_mesh3_clone(
            world,
            character);
    zox_add(mesh_clone, VoxMesh);
    zox_set_unique_name(mesh_clone, "character_clone");
    zox_setv(mesh_clone, Position3D, renderer_position);
    zox_setv(mesh_clone, Rotation3D, quaternion_identity);
    add_eternal_euler(
        world,
        mesh_clone,
        (float3) { 0, rotate_speed, 0 });
    dbg_render_texture_mesh = mesh_clone;
    // focuses on actual character
    // dbg_render_texture_mesh = character;
    // renderer_position = zox_getv(character, Position3D);
    // testing
    // renderer_position = zox_getv(character, Position3D);
    // zox_setv(mesh_clone, Position3D, renderer_position);
    // TODO: Pass in closebutton=1
    entity3 e2 = spawn_window(
        world,
        prefab_window,
        prefab_body,
        "Character",
        canvas,
        position,
        size,
        anchor,
        header_font_size,
        header_padding,
        NULL);
    entity e = e2.x;
    entity body = e2.z;
    zox_add(e, MenuMirror);
    entity ui = spawn_render_frame_at(
        world,
        body,
        float2_centre,
        int2_zero,
        size,
        mesh_clone,
        renderer_position,
        downscale,
        alpha);
    zox_set_parent(world, mesh_clone, ui);
    dbg_render_texture_character = e;
    return e;
}

void zox_tst_render_texture_character2(
    ecs *world,
    ClickEventData data)
{
    entity player = dbg_player;
    zox_tst_render_texture_character(
        world,
        player);
}
