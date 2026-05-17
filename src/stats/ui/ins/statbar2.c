entity spawn_statbar2(ecs *world, entity canvas, entity parent, entity2 character_group, color_rgb c, byte layer, float2 anchor, int2 size, int2 position, byte font_size) {
    entity stat = character_group.y;
    if (!zox_valid(stat)) {
        zox_log_error("stat null [spawn_statbar2]");
        return 0;
    }
    byte2 padding = (byte2) { 16, 8 };    // text?
    color back_color = (color) { c.r / 2, c.g / 2, c.b / 2, back_bar_color.a };
    color front_color = (color) { c.r, c.g, c.b, back_bar_color.a };
    int2 canvas_size = zox_get_value(canvas, LayoutSize);
    entity2 e2 = spawn_elementbar2(world, prefab_statbar2D, canvas, parent, position, size, font_size, padding, anchor, layer, int2_half(canvas_size), canvas_size, 0, back_color, front_color);
    zox_set_unique_name(e2.x, "statbar2");
    zox_set_unique_name(e2.y, "statbar2_text");
    zox_set(e2.x, StatLink, { stat });
    zox_set(e2.y, StatLink, { stat });
    zox_add_tag(e2.y, StatsLabel);
    return e2.x;
}
