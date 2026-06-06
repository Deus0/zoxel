entity spawn_prefab_body(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("body");
    zox_add_tag(e, WindowBody);
    // Texture
    zox_add_tag(e, FixToLayout);
    add_frame_texture_type(world, e, window_fill, window_outline, default_button_corner, default_button_frame_thickness);
    // Selection
    zox_add_tag(e, Selectable);
    zox_prefab_set(e, SelectState, { zox_select_state_none });
    // Clicking
    zox_add_tag(e, Clickable);
    zox_prefab_set(e, ClickState, { 0 });
    zox_prefab_set(e, Clicker, { 0 });
    zox_prefab_set(e, ClickEvent, { NULL });
    zox_prefab_set(e, ClickDisabled, { 0 });
    return e;
}
