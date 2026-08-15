void initialize_layout2(ecs *world, entity e, entity parent, int2 position, int2 size, float2 anchor, byte layer) {
    zox_set_parent(world, e, parent);
    zox_set(e, Anchor, { anchor });
    zox_set(e, Layer, { layer });
    zox_set(e, LayoutSize, { size });
    zox_set(e, LayoutPosition, { position });
    // zox_set(e, CanvasLink, { canvas });
    // zox_set(e, ParentLink, { parent });
    // Where we link to canvas children
    /*if (canvas == parent) {
        zox_set(canvas, WindowToTop, { e });
    }*/
}

entity spawn_layout2(ecs *world, entity prefab, entity parent, int2 position, int2 size, float2 anchor, byte layer) {
    zox_instance(prefab);
    zox_name("layout2");
    initialize_layout2(world, e, parent, position, size, anchor, layer);
    return e;
}

entity spawn_layout2_on_canvas(ecs *world, entity prefab, entity canvas, int2 position, int2 size, float2 anchor) {
    return spawn_layout2(world, prefab, canvas, position, size, anchor, 0);
}
