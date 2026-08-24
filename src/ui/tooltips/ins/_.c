extern entity spawn_label(ecs *world, entity prefab, entity parent, int2 position, float2 anchor, byte2 padding, const char* text, byte font_size, byte alignment, byte layer, color fill, color fillo, color font_fill, color font_fillo);

color tooltip_fill = color_white;
color tooltip_fillo = color_black;
color tooltip_font_fill = color_black;
color tooltip_font_fillo = color_white;

entity spawn_tooltip(
    ecs *world,
    entity prefab,
    entity canvas)
{
    // Sizing
    byte font_size = 5 * ui_scale;
    byte2 padding = (byte2) { 3 * ui_scale, 2  * ui_scale };
    int2 position = (int2) { -2 * ui_scale, -2 * ui_scale };
    // Misc
    entity parent = canvas;
    int layer = max_layers2D - 2;
    byte alignment = zox_alignment_top_right;
    float2 anchor = float2_one;
    // le spawn
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
        tooltip_fill,
        tooltip_fillo,
        tooltip_font_fill,
        tooltip_font_fillo);
    zox_name("tooltip");
    zox_setv(e, RenderDisabled, 1);
    zox_setv(e, OutlineThickness, 0);
    if (local_mouse) {
        entity pointer = zox_get_child_by_id(world, local_mouse, zox_id(ZevicePointer));
        if (zox_valid(pointer)) {
            zox_setv(e, ZeviceLink, pointer);
        } else {
            zox_loge("Mouse has no Pointer child");
        }
    }
    return e;
}
