const color back_bar_color = { 66, 12, 12, 155 };
const color front_bar_color = { 188, 25, 25, 155 };

entity spawn_statbar2(
    ecs* world,
    entity parent,
    entity stat,
    int2 position,
    int2 size,
    float2 anchor,
    color_rgb fill,
    byte font_size)
{
    if (!zox_valid(stat)) {
        zox_loge("stat null [spawn_statbar2]");
        return 0;
    }
    byte2 padding = (byte2) { 16, 8 };    // text?
    color back_color = (color) { fill.r / 2, fill.g / 2, fill.b / 2, back_bar_color.a };
    color front_color = (color) { fill.r, fill.g, fill.b, back_bar_color.a };
    entity2 e2 = spawn_ui_bar(
        world,
        prefab_statbar2,
        parent,
        position,
        size,
        anchor,
        font_size,
        padding,
        back_color,
        front_color);
    zox_set_unique_name(e2.x, "statbar2");
    zox_set_unique_name(e2.y, "statbar2_text");
    zox_set(e2.x, StatLink, { stat });
    zox_set(e2.y, StatLink, { stat });
    zox_add(e2.y, StatsLabel);
    return e2.x;
}
