// just a draggable button left and right only
// just a panel with a label and a bar
// the bar can be clicked to reposition the handle as well
entity spawn_prefab_handle(
    ecs* world,
    entity prefab)
{
    zox_prefab_child(prefab);
    zox_prefab_name("handle");
    // Drag
    zox_add(e, Dragable);
    zox_setv(e, DraggableState, 0);
    zox_setv(e, DraggingDelta, int2_zero);
    zox_setv(e, DraggerLink, 0);
    zox_setv(e, DraggedLink, 0);
    zox_setv(e, LayoutConstraints, int4_zero);
    // Slide
    zox_setv(e, SlideEvent, 0);
    zox_set(e, SlideBounds, { (float2) { 0, 1 }});
    // set texture data
    zox_add(e, FixToLayout);
    return e;
}
