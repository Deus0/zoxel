entity spawn_fps_display(
    ecs* world,
    entity canvas)
{
    if (!zox_valid(canvas) ||
        !zox_has(canvas, Canvas))
    {
        zox_loge("Invalid Canvas");
        return 0;
    }
    byte layer = max_layers2D - 10; // editor_overlay_layer + 10;
    byte font_size = 32;
    byte2 padding = (byte2) { 12, 8 };
    float2 anchor = float2_top_right;
    int2 position = (int2) { -16, -16 };
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
    zox_add(e, FPSDisplay);
    zox_setv(e, FPSDisplayTicker, 0);
    return e;
}

// z: toggles the fps display
void toggle_fps_viewer(ecs *world, int32_t keycode) {
    if (keycode != zox_key_z) {
        return;
    }
    const entity canvas = get_editor_canvas(world);
    if (canvas) {
        canvas_toggle_ui(world, canvas, zox_get_child_by_id(world, canvas, zox_id(FPSDisplay)), spawn_fps_display);
    }
}
