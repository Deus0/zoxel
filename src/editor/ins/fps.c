entity spawn_fps_display(ecs *world, entity canvas) {
    if (!zox_valid(canvas) || !zox_has(canvas, Canvas)) {
        zox_log_error("Invalid Canvas");
        return 0;
    }
    byte layer = max_layers2D - 10; // editor_overlay_layer + 10;
    byte font_size = 32;
    byte2 padding = (byte2) { 12, 8 };
    float2 anchor = { 1.0f, 1.0f };
    int2 position = (int2) { -16, -16 };
    // int2 psize = zox_getv(canvas, LayoutSize);
    entity e = spawn_label(
        world,
        prefab_label2t,
        canvas,
        position,
        anchor,
        padding,
        "",
        font_size,
        zox_alignment_top_right,
        layer,
        button_fill,
        button_outline,
        button_font_fill,
        button_font_outline);
    zox_set_unique_name(e, "fps_viewer");
    zox_prefab_name("fps_display");
    //zox_prefab_set(e, FontOutlineColor, {{ 200, 80, 80, 255 }});
    //zox_prefab_set(e, FontFillColor, {{ 244, 33, 33, 255 }});
    zox_add(e, FPSDisplay);
    zox_prefab_set(e, FPSDisplayTicker, { 0 });
    return e;
}

void toggle_fps_viewer(ecs *world, int32_t keycode) {
    if (keycode != zox_key_z) {
        return;
    }
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    entity canvas = zox_get_link(world, player, Canvas);
    if (!zox_valid(canvas)) {
        return;
    }
    entity fps_viewer = zox_get_child_by_id(world, canvas, zox_id(FPSDisplay));
    if (fps_viewer) {
        zox_delete(fps_viewer);
    } else {
        spawn_fps_display(world, canvas);
    }
    // zox_log("FPSViewer %s", !fps_viewer ? "Enabled" : "Disabled");
}
