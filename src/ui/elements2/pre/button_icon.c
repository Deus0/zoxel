entity spawn_prefab_button_icon(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("button_icon");
    zox_add(e, Button);
    //  interactable
    zox_add(e, ClickMakeSound);
    zox_add(e, Selectable);
    zox_prefab_set(e, SelectState, { 0 });
    zox_add(e, Clickable);
    zox_prefab_set(e, ClickState, { 0 });
    zox_prefab_set(e, Clicker, { 0 });
    zox_prefab_set(e, ClickEvent, { NULL });
    zox_prefab_set(e, ClickDisabled, { 0 });
    // texture
    zox_set(e, FrameCorner, { default_button_corner });
    zox_set(e, OutlineThickness, { default_button_frame_thickness });
    zox_set(e, FillColor, { default_fill_color });
    zox_set(e, OutlineColor, { default_outline_color });
    return e;
}
