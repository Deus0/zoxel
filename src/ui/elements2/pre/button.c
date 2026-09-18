entity spawn_prefab_button(
    ecs* world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("button");
    zox_add(e, Button);
    //  Select
    zox_add(e, Selectable);
    zox_setv(e, SelectState, 0);
    // Click
    zox_add(e, Clickable);
    zox_add(e, ClickMakeSound);
    zox_setv(e, ClickState, 0);
    zox_setv(e, Clicker, 0);
    zox_setv(e, ClickEvent, NULL);
    zox_setv(e, ClickDisabled, 0);
    // texture
    zox_add(e, FixToLayout);
    zox_setv(e, FrameCorner, default_button_corner);
    zox_setv(e, OutlineThickness, default_button_frame_thickness);
    // Colors
    zox_setv(e, FillColor, button_fill);
    zox_setv(e, OutlineColor, button_outline);
    zox_setv(e, ElementFillColor, button_fill);
    zox_setv(e, ElementOutlineColor, button_outline);
    zox_setv(e, SelectedFillColor, button_fill_selected);
    zox_setv(e, SelectedOutlineColor, button_outline_selected);
    return e;
}
