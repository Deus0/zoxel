void initialize_layout2(ecs *world, entity e, entity parent, entity canvas, int2 position, int2 size, float2 anchor, byte layer) {
    zox_set_parent(world, e, parent);
    zox_set(e, Anchor, { anchor });
    zox_set(e, Layer2D, { layer });
    zox_set(e, LayoutSize, { size });
    zox_set(e, LayoutPosition, { position });
    // zox_set(e, CanvasLink, { canvas });
    // zox_set(e, ParentLink, { parent });
    // Where we link to canvas children
    if (canvas == parent) {
        zox_set(canvas, WindowToTop, { e });
    }
}

void set_element_spawn_data(ecs *world, entity e, LayoutParentData canvas_data, LayoutParentData parent_data, ElementSpawnData element_data) {
    // zox_set(e, ParentLink, { parent_data.e });
    zox_set_parent(world, e, parent_data.e);
    zox_set(e, LayoutPosition, { element_data.position });
    zox_set(e, LayoutSize, { element_data.size });
    zox_set(e, Anchor, { element_data.anchor });
    zox_set(e, Layer2D, { element_data.layer });
    zox_set(e, CanvasLink, { canvas_data.e });
    // TODO: Canvas Dirty Flag?
    if (canvas_data.e == parent_data.e) {
        zox_set(canvas_data.e, WindowToTop, { e });
    }
}
