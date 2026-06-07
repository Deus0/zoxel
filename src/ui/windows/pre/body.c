entity spawn_prefab_body(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("body");
    zox_add_tag(e, WindowBody);
    // Texture
    zox_add_tag(e, FixToLayout);
    zox_add_tag(e, FrameTexture);
    zox_prefab_set(e, FrameCorner, { default_button_corner });
    zox_prefab_set(e, OutlineThickness, { default_button_frame_thickness });
    zox_prefab_set(e, FillColor, { window_fill });
    zox_prefab_set(e, OutlineColor, { window_outline });
    // Selection
    zox_add_tag(e, Selectable);
    zox_prefab_set(e, SelectState, { 0 });
    // Clicking
    zox_add_tag(e, Clickable);
    zox_prefab_set(e, ClickState, { 0 });
    zox_prefab_set(e, Clicker, { 0 });
    zox_prefab_set(e, ClickEvent, { NULL });
    zox_prefab_set(e, ClickDisabled, { 0 });
    return e;
}
