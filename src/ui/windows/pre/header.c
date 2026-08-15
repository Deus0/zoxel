entity spawn_prefab_header(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("header");
    zox_add(e, Header);
    // Texture
    zox_add(e, FixToLayout);
    zox_add(e, FrameTexture);
    zox_prefab_set(e, FrameCorner, { header_corner_size });
    zox_prefab_set(e, OutlineThickness, { header_outline_size });
    zox_prefab_set(e, FillColor, { header_fill });
    zox_prefab_set(e, OutlineColor, { header_outline });
    zox_prefab_set(e, ElementFillColor, { header_fill });
    zox_prefab_set(e, ElementOutlineColor, { header_outline });
    // Select
    /*zox_add(e, Selectable);
    zox_prefab_set(e, SelectState, { 0 });
    zox_prefab_set(e, SelectedFillColor, { button_fill_selected });
    zox_prefab_set(e, SelectedOutlineColor, { button_outline_selected });
    // Click
    zox_add(e, Clickable);
    zox_prefab_set(e, ClickState, { 0 });
    zox_prefab_set(e, ClickDisabled, { 0 });
    zox_prefab_set(e, Clicker, { 0 });
    zox_prefab_set(e, ClickEvent, { NULL });*/
    // Dragging
    zox_add(e, Dragable);
    zox_prefab_set(e, DraggableState, { 0 });
    zox_prefab_set(e, DraggingDelta, { int2_zero });
    zox_prefab_set(e, DraggerLink, { 0 });
    zox_prefab_set(e, DraggedLink, { 0 });
    return e;
}
