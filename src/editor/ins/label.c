entity spawn_game_debug_label(ecs *world, entity canvas) {
    if (!canvas || !zox_has(canvas, PlayerLink)) {
        zox_loge("[spawn_game_debug_label] error: invalid canvas, or no player found on canvas");
        return 0;
    }
    entity prefab = prefab_label_textured;
    if (!zox_valid(prefab)) {
        zox_loge("prefab invalid in [spawn_game_debug_label]");
        return 0;
    }
    int layer = editor_overlay_layer;
    zox_geter(canvas, PlayerLink, player);
    color fill = button_fill;
    color font_fill = button_font_fill;
    fill.a = 222;
    font_fill.a = 255;
    entity parent = canvas;
    byte font_size = 16;
    byte2 padding = (byte2) { 12, 8 };
    byte alignment = zox_alignment_top_right;
    float2 anchor = float2_one; //  { 1.0f, 1.0f };
    int2 position = (int2) { -8, -8 };
    entity e = spawn_label(world, prefab, parent, position, anchor, padding, "", font_size, alignment, layer, fill, button_outline, font_fill, button_font_outline);
    zox_set_unique_name(e, "debug_label");
    zox_add(e, EditorElement);
    zox_add(e, GameDebugLabel);
    zox_set(e, DebugLabelData, { local_debug_label });
    zox_set(e, PlayerLink, { player->value });
    return e;
}
