entity spawn_image(ecs* world, entity prefab, entity parent, entity canvas, int2 position, int2 size, float2 panchor, byte layer, byte rdisabled, entity texture) {
    zox_instance(prefab);
    zox_name("image");
    zox_set(e, LayoutPosition, { position });
    zox_set(e, LayoutSize, { size });
    zox_set(e, Anchor, { panchor });
    zox_set(e, Layer2D, { layer });
    zox_set_parent(world, e, parent);
    zox_set(e, CanvasLink, { canvas });
    zox_set(e, RenderDisabled, { rdisabled });
    if (canvas == parent) {
        // on_child_added(world, canvas, e);
        zox_set(canvas, WindowToTop, { e });
    }
    clone_texture_data(world, e, texture);
    return e;
}
