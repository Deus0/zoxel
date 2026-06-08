// just a draggable button left and right only
// just a panel with a label and a bar
// the bar can be clicked to reposition the handle as well
entity spawn_prefab_handle(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("handle");
    // interactable
    // Drag
    zox_add_tag(e, Dragable);
    zox_prefab_set(e, DraggableState, { 0 });
    zox_prefab_set(e, DraggingDelta, { int2_zero });
    zox_prefab_set(e, DraggerLink, { 0 });
    zox_prefab_set(e, DraggedLink, { 0 });
    zox_prefab_set(e, LayoutConstraints, { int4_zero });
    // Slide
    zox_prefab_set(e, SlideEvent, { 0 });
    zox_prefab_set(e, SlideBounds, { (float2) { 0, 1 }});
    // set texture data
    zox_add_tag(e, FixToLayout);
    return e;
}
