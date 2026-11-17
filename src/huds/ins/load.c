// the "Loading" ui: 2D text on the canvas, or on 3D huds (XR) a 3D label ahead of the head (huds3_follow_system)
//  - either way the canvas's child, so huds/eve/load.c finds and deletes it the same
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
        e = spawn_text(
            world,
            prefab_text,
            canvas,
            int2_zero,
            float2_half,
            font_size,
            zox_alignment_centre,
            padding,
            "Loading",
            fill,
            outline);
        zox_setv(e, Layer, get_game_overlay_layer());
    }
    zox_set_unique_name(e, "load_ui");
    zox_add(e, LoadUI);
    return e;
}
