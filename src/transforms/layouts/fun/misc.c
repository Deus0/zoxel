// OBSOLETE
void set_element_spawn_data(
    ecs *world,
    entity e,
    LayoutParentData canvas_data,
    LayoutParentData parent_data,
    ElementSpawnData element_data)
{
    zox_set_parent(world, e, parent_data.e);
    zox_set(e, LayoutPosition, { element_data.position });
    zox_set(e, LayoutSize, { element_data.size });
    zox_set(e, Anchor, { element_data.anchor });
    zox_set(e, Layer, { element_data.layer });
    // zox_set(e, CanvasLink, { canvas_data.e });
    // TODO: Canvas Dirty Flag?
    if (canvas_data.e == parent_data.e) {
        zox_set(canvas_data.e, WindowToTop, { e });
    }
}
