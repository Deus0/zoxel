entity spawn_prefab_button_icon(
    ecs* world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("button_icon");
    zox_add(e, Button);
    //  interactable
    zox_add(e, ClickMakeSound);
    zox_add(e, Selectable);
    zox_setv(e, SelectState, 0);
    zox_add(e, Clickable);
    zox_setv(e, ClickState, 0);
    zox_setv(e, Clicker, 0);
    zox_setv(e, ClickEvent, NULL);
    zox_setv(e, ClickDisabled, 0);
    // texture
    zox_setv(e, FrameCorner, default_button_corner);
    zox_setv(e, OutlineThickness, default_button_frame_thickness);
    zox_setv(e, FillColor, default_fill_color);
    zox_setv(e, OutlineColor, { default_outline_color);
    return e;
}
