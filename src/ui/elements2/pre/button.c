entity spawn_prefab_button(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("button");
    zox_add_tag(e, Button);
    // texture
    zox_add_tag(e, FixToLayout);
    zox_prefab_set(e, FrameCorner, { default_button_corner });
    zox_prefab_set(e, OutlineThickness, { default_button_frame_thickness });
    // Colors
    zox_prefab_set(e, FillColor, { button_fill });
    zox_prefab_set(e, OutlineColor, { button_outline });
    zox_prefab_set(e, ElementFillColor, { button_fill });
    zox_prefab_set(e, ElementOutlineColor, { button_outline });
    zox_prefab_set(e, SelectedFillColor, { button_fill_selected });
    zox_prefab_set(e, SelectedOutlineColor, { button_outline_selected });
    // zox_add_tag(e, TextResizer);
    //  Select
    zox_add_tag(e, Selectable);
    zox_prefab_set(e, SelectState, { 0 });
    // Click
    zox_add_tag(e, Clickable);
    zox_add_tag(e, ClickMakeSound);
    zox_prefab_set(e, ClickState, { 0 });
    zox_prefab_set(e, Clicker, { 0 });
    zox_prefab_set(e, ClickEvent, { NULL });
    zox_prefab_set(e, ClickDisabled, { 0 });
    return e;
}
