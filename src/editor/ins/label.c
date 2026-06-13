entity spawn_game_debug_label(ecs *world, entity canvas) {
    if (!canvas || !zox_has(canvas, PlayerLink)) {
        zox_log("! [spawn_game_debug_label] error: invalid canvas, or no player found on canvas\n");
        return 0;
    }
    zox_geter(canvas, PlayerLink, player);
    color fill = button_fill;
    color font_fill = button_font_fill;
    fill.a = 222;
    font_fill.a = 255;
    entity parent = canvas;
    int layer = 1;
    byte font_size = 16;
    byte2 padding = (byte2) { 12, 8 };
    byte alignment = zox_alignment_top_right;
    float2 anchor = float2_one; //  { 1.0f, 1.0f };
    int2 position = (int2) { -8, -8 };
    // returns the child zext
    entity prefab = prefab_label_textured; // prefab_game_debug_label
    entity e = spawn_label(world, prefab, parent, position, anchor, padding, "", font_size, alignment, layer, fill, button_outline, font_fill, button_font_outline);
    zox_add_tag(e, EditorElement);
    zox_add_tag(e, GameDebugLabel);
    zox_set(e, DebugLabelData, { local_debug_label });
    zox_set(e, PlayerLink, { player->value });
    return e;
}
