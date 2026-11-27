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

// z: flips the "FPS" setting (saved), fps_display_show_system shows or hides the display
void toggle_fps_viewer(ecs *world, int32_t keycode) {
    if (keycode != zox_key_z || !zox_valid(main_app)) {
        return;
    }
    // set before the setting's Dirty: its observer (editor_settings_observer_callback) can run right away and set it too
    const byte on = !zox_fps_display;
    zox_fps_display = on;
    iter it = zox_children(world, main_app);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity e = it.entities[i];
            if (zox_has(e, SettingFPSDisplay)) {
                zox_set(e, SettingByte, { on });
                zox_add(e, Dirty);
                zox_add(e, Refresh);   // the save
            }
        }
    }
}
