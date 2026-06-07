entity spawn_prefab_grid(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("grid");
    // Texture
    zox_add_tag(e, FixToLayout);
    zox_add_tag(e, FrameTexture);
    zox_prefab_set(e, FrameCorner, { default_button_corner });
    zox_prefab_set(e, OutlineThickness, { default_button_frame_thickness });
    zox_prefab_set(e, FillColor, { window_fill });
    zox_prefab_set(e, OutlineColor, { window_outline });
    // body stuff
    zox_add_tag(e, Selectable);
    zox_prefab_set(e, SelectState, { 0 });
    // Clicking
    zox_add_tag(e, Clickable);
    zox_prefab_set(e, ClickState, { 0 });
    zox_prefab_set(e, Clicker, { 0 });
    zox_prefab_set(e, ClickEvent, { NULL });
    zox_prefab_set(e, ClickDisabled, { 0 });
    // Grid
    zox_prefab_set(e, GridDirty, { zox_dirty_trigger });
    zox_prefab_set(e, GridPadding, { byte2_zero });
    zox_prefab_set(e, GridMargins, { byte2_zero });
    return e;
}
