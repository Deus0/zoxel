entity spawn_prefab_scrollbar_handle(ecs *world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("scrollbar_handle");
    zox_add_tag(e, Scrollbar);
    zox_prefab_set(e, ScrollviewLink, { 0 });
    // Selection
    /*zox_add_tag(e, Selectable);
    zox_prefab_set(e, SelectState, { 0 });
    // Clicking
    zox_add_tag(e, Clickable);
    zox_prefab_set(e, ClickState, { 0 });
    zox_prefab_set(e, Clicker, { 0 });
    zox_prefab_set(e, ClickEvent, { NULL });
    zox_prefab_set(e, ClickDisabled, { 0 });
    // Dragging
    zox_add_tag(e, Dragable);
    zox_prefab_set(e, DraggableState, { 0 });
    zox_prefab_set(e, DraggingDelta, { int2_zero });
    zox_prefab_set(e, DraggerLink, { 0 });
    zox_prefab_set(e, DraggedLink, { 0 });
    zox_prefab_set(e, LayoutConstraints, { int4_zero });
    zox_add_tag(e, FixToLayout);*/
    // zox_set(e, FillColor, { scrollbar_front_color });
    return e;
}
