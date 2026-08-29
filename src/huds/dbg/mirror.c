entity dbg_render_texture_character = 0;
entity dbg_render_texture_mesh = 0;
extern entity get_linked_character(ecs*, entity);

entity spawn_mesh3_clone(
    ecs* world,
    entity clonee)
{
    // get mesh data
    const MeshIndicies* source_indicies = zox_get(clonee, MeshIndicies);
    const MeshVertices* source_verts = zox_get(clonee, MeshVertices);
    const MeshColorRGBs* source_colors = zox_get(clonee, MeshColorRGBs);
    // Initialize new data
    MeshIndicies indicies = { 0 };
    MeshVertices verts = { 0 };
    MeshColorRGBs colors = { 0 };
    initialize_MeshIndicies(
        &indicies,
        source_indicies->length);
    initialize_MeshVertices(
        &verts,
        source_verts->length);
    initialize_MeshColorRGBs(
        &colors,
        source_colors->length);
    if (!indicies.value ||
        !verts.value ||
        !colors.value)
    {
        zox_loge("Mesh3 data malloc failed");
        return 0;
    }
    memcpy(
        indicies.value,
        source_indicies->value,
        source_indicies->length * sizeof(int));
    memcpy(
        verts.value,
        source_verts->value,
        source_verts->length * sizeof(float3));
    memcpy(
        colors.value,
        source_colors->value,
        source_colors->length * sizeof(color_rgb));
    entity e = zox_ins(world, prefab_mesh3);
    zox_set_ptr(e, MeshIndicies, indicies);
    zox_set_ptr(e, MeshVertices, verts);
    zox_set_ptr(e, MeshColorRGBs, colors);
    zox_setv(e, MeshRenderCount, zox_getv(clonee, MeshRenderCount));
    zox_add(e, MeshDirty);
    return e;
}

entity zox_tst_render_texture_character(
    ecs *world,
    entity player)
{
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
    // entity player = dbg_player;
    if (!zox_valid(player)) {
        return 0;
    }
    entity canvas = get_linked_canvas(world, player);
    entity character = get_linked_character(world, player);
    if (!zox_valid(character)) {
        zox_log("No character to render");
        return 0;
    }
    const float3 renderer_position = (float3) {
        0,
        -5, // -50,
        0
    };
    int padding = 40;
    int2 size = int2_single(128 * ui_scale);
    // int2 tsize = int2_scale1(lsize, 1 / downscale);
    int2 position = (int2) { 0, 0 }; // -size.x / 2, -size.y / 2 };
    position.x -= padding;
    position.y -= padding;
    float2 anchor = float2_centre; // float2_top_right; // (float2) { 1, 1 };
    byte header_font_size = 8 * ui_scale;
    byte2 header_padding = (byte2) {
        10 * ui_scale,
        4 * ui_scale
    };
    dbg_render_texture_mesh =
        spawn_mesh3_clone(
            world,
            character);
    zox_add(dbg_render_texture_mesh, VoxMesh);
    zox_set_unique_name(dbg_render_texture_mesh, "character_clone");
    zox_setv(dbg_render_texture_mesh, Position3D, renderer_position);
    zox_setv(dbg_render_texture_mesh, Rotation3D, quaternion_identity);
    // dbg_render_texture_mesh = character;
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
    spawn_render_frame_at(
        world,
        body,
        float2_centre,
        int2_zero,
        size,
        dbg_render_texture_mesh,
        renderer_position);
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
