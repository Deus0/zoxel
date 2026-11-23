// the "Loading" ui: 2D text on the canvas, or on 3D huds (XR) a 3D label ahead of the head (huds3_follow_system)
//  - either way the canvas's child, so huds/eve/load.c finds and deletes it the same
//  - 2D: a layout holding two texts, "Loading" and under it the near chunks loaded (Chunks [loaded/near],
//    load_ui_chunks_system); a layout, as a text's children are its glyphs
entity spawn_load_ui3(
    ecs* world,
    entity player,
    entity canvas)
{
    color fill = button_fill;
    fill.a = 177;
    float3 position;
    float4 rotation;
    huds3_pose(world, player, huds3_distance, &position, &rotation);
    entity e = spawn_label3(
        world,
        "Loading",
        18 * ui_scale,
        fill,
        button_outline,
        button_font_fill,
        button_font_outline,
        zox_ui_scale3,
        0,
        0).x;
    zox_setv(e, Position3D, position);
    zox_setv(e, Rotation3D, rotation);
    zox_setv(e, Scale1, huds3_label_height * 2);     // alone on black, read at a glance
    zox_set_parent(world, e, canvas);
    zox_link(world, e, PlayerLink, player);
    return e;
}

entity spawn_load_ui(
    ecs* world,
    entity player,
    entity canvas)
{
    if (!zox_valid(canvas)) {
        return 0;
    }
    entity e;
    if (zox_huds3D) {
        e = spawn_load_ui3(world, player, canvas);
    } else {
        color fill = (color) { 44, 222, 222, 222 };
        color outline = (color) { 0, 255, 255, 244 };
        byte font_size = ui_scale * 24;
        byte2 padding = byte2_single(1 * ui_scale);
        const byte chunks_font_size = ui_scale * 12;
        const int2 size = { font_size * 8, font_size * 2 + chunks_font_size * 2 };
        e = spawn_layout2(world, prefab_layout2, canvas, int2_zero, size, float2_half, get_game_overlay_layer());
        const entity label = spawn_text(
            world,
            prefab_text,
            e,
            int2_zero,
            float2_half,
            font_size,
            zox_alignment_centre,
            padding,
            "Loading",
            fill,
            outline);
        zox_setv(label, Layer, get_game_overlay_layer());
        const entity chunks = spawn_text(
            world,
            prefab_text,
            e,
            (int2) { 0, -(font_size + chunks_font_size) },
            float2_half,
            chunks_font_size,
            zox_alignment_centre,
            padding,
            "Chunks [0/0]",
            fill,
            outline);
        zox_setv(chunks, Layer, get_game_overlay_layer());
        zox_add(chunks, LoadUIChunks);
        zox_stream_end_loaded = 0;  // not the last load's, till stream_end_system counts this one
        zox_stream_end_near = 0;
    }
    zox_set_unique_name(e, "load_ui");
    zox_add(e, LoadUI);
    return e;
}
