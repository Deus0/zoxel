entity spawn_game_debug_label(ecs *world, entity canvas) {
    if (!canvas || !zox_has(canvas, PlayerLink)) {
        zox_log("! [spawn_game_debug_label] error: invalid canvas, or no player found on canvas\n");
        return 0;
    }
    entity parent = canvas;
    int layer = 1;
    byte font_size = 16;
    byte2 padding = (byte2) { 12, 8 };
    byte alignment = zox_alignment_top_right;
    float2 anchor = float2_one; //  { 1.0f, 1.0f };
    int2 pixel_position = (int2) { -8, -8 };
    int2 parent_pixel_size = zox_get_value(parent, LayoutSize);
    // returns the child zext
    entity zext = spawn_label_background(world, prefab_game_debug_label, parent, canvas, pixel_position, anchor, padding, "", font_size, alignment, layer, int2_half(parent_pixel_size), parent_pixel_size, button_fill, button_outline, window_fill, window_outline, 0);
    zox_geter(canvas, PlayerLink, playerLink);
    zox_set(zext, PlayerLink, { playerLink->value });
    return zext;
}
