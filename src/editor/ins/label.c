entity spawn_game_debug_label(ecs* world, entity canvas) {
    if (!canvas || !zox_has(canvas, PlayerLink)) {
        zox_loge("[spawn_game_debug_label] error: invalid canvas, or no player found on canvas");
        return 0;
    }
    entity prefab = prefab_label_textured;
    if (!zox_valid(prefab)) {
        zox_loge("prefab invalid in [spawn_game_debug_label]");
        return 0;
    }
    // Lyout
    entity parent = canvas;
    int layer = editor_overlay_layer;
    byte alignment = zox_alignment_top_right;
    float2 anchor = float2_one; //  { 1.0f, 1.0f };
    int2 position = (int2) { -8, -8 };
    // Style
    color fill = button_fill;
    color font_fill = button_font_fill;
    fill.a = 222;
    font_fill.a = 255;
    byte font_size = 6 * ui_scale;
    byte2 padding = (byte2) { 6 * ui_scale, 6 * ui_scale };
    entity e = spawn_label(
        world,
        prefab,
        parent,
        position,
        anchor,
        padding,
        "",
        font_size,
        alignment,
        layer,
        fill,
        button_outline,
        font_fill,
        button_font_outline);
    zox_set_unique_name(e, "debug_label");
    zox_add(e, EditorElement);
    zox_add(e, GameDebugLabel);
    zox_setv(e, FrameCorner, 0);
    zox_setv(e, DebugLabelData, local_debug_label);
    zox_setv(e, PlayerLink, zox_getv(canvas, PlayerLink));
    return e;
}
