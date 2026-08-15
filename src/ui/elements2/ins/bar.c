// TODO: Pass in more parameters for 3D too
entity2 spawn_ui_bar(
    ecs* world,
    entity prefab,
    entity parent,
    int2 position,
    int2 size,
    float2 position_anchor,
    byte font_size,
    byte2 text_padding,
    color back_color,
    color front_color)
{
    entity2 output;
    entity prefab_frontbar = prefab_element_frame;
    entity prefab_bar_text = prefab_text;
    color font_outline = (color) { 11, 11, 11, 222 };
    color font_fill = (color) { 225, 225, 225, 255 };
    entity e = spawn_uic(
        world,
        prefab,
        parent,
        position_anchor,
        position,
        size,
        size,
        back_color,
        color_black);
    output.x = e;
    zox_name("bar2");
    {
        float2 bar_anchor = float2_centre;
        entity e2 = spawn_uic(
            world,
            prefab_frontbar,
            e,
            bar_anchor,
            int2_zero,
            size,
            size,
            front_color,
            color_black);
        zox_set_unique_name(e2, "bar2_frontbar");
        zox_add(e2, BarFront);
    }
    {
        entity e2 = spawn_text(
            world,
            prefab_bar_text,
            e,
            int2_zero,
            float2_half,
            font_size,
            zox_alignment_centre,
            text_padding,
            "",
            font_fill,
            font_outline);
        zox_set_unique_name(e2, "bar2_text");
        output.y = e2;
    }
    return  output;
}
