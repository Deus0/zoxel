entity spawn_button_icon(
    ecs *world,
    entity p,
    entity texture,
    entity canvas,
    entity parent,
    int2 position,
    int2 size,
    float2 panchor,
    byte layer,
    color cfill,
    color coutline,
    byte rdisabled,
    ClickEvent onclick,
    byte padding
) {

    zox_instance(p);
    zox_name("button_icon");

    zox_set(e, LayoutPosition, { position });
    zox_set(e, LayoutSize, { size });
    zox_set(e, Anchor, { panchor });

    zox_set(e, Layer2D, { layer });
    // zox_set(e, ParentLink, { parent });
    zox_set(e, CanvasLink, { canvas });
    zox_set_parent(world, e, parent);

    if (canvas == parent) {
        // on_child_added(world, canvas, e);
        zox_set(canvas, WindowToTop, { e });
    }

    zox_set(e, RenderDisabled, { rdisabled });

    zox_set(e, Color, { cfill });
    zox_set(e, OutlineColor, { coutline });

    zox_set(e, ClickEvent, { onclick.value });

    // Children children = (Children) { 0 };

    // spawn texture ui image here
    entity image = spawn_image(
        world,
        prefab_element_ready, texture,
        canvas,
        e,
        int2_zero,
        (int2) { size.x - padding, size.y - padding },
        float2_half,
        layer + 1,
        rdisabled
    );
    zox_set_parent(world, image, e);
    //add_to_Children(&children, image);
    //zox_set_ptr(e, Children, children);

    return e;
}
