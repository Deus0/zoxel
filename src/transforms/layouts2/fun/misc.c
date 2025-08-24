void initialize_element_invisible(
    ecs *world,
    const entity e,
    const entity parent,
    const entity canvas,
    const int2 pixel_position,
    const int2 pixel_size,
    const float2 anchor,
    const byte layer,
    const float2 position2,
    const int2 pixel_positionv
) {
    zox_set(e, Anchor, { anchor });
    zox_set(e, Layer2D, { layer });
    zox_set(e, LayoutSize, { pixel_size });
    zox_set(e, LayoutPosition, { pixel_position });
    zox_set(e, CanvasLink, { canvas });
    zox_set(e, ParentLink, { parent });
    if (canvas == parent) {
        on_child_added(world, canvas, e);
        zox_set(canvas, WindowToTop, { e });
    }

    /*
    zox_set(e, Position2, { position2 }); // set this inside pixel position system
    zox_set(e, CanvasPosition, { pixel_positionv }); // set this inside system too
    */
}

void initialize_element(
    ecs *world,
    const entity e,
    const entity parent,
    const entity canvas,
    const int2 pixel_position,
    const int2 pixel_size,
    const int2 texture_size,
    const float2 anchor,
    const byte layer,
    const float2 position2,
    const int2 pixel_positionv
) {
    initialize_element_invisible(
        world,
        e,
        parent,
        canvas,
        pixel_position,
        pixel_size,
        anchor,
        layer,
        position2,
        pixel_positionv);
}

void set_element_spawn_data(
    ecs *world,
    const entity e,
    const LayoutParentData canvas_data,
    const LayoutParentData parent_data,
    const ElementSpawnData element_data
) {
    zox_set(e, LayoutPosition, { element_data.position });
    zox_set(e, LayoutSize, { element_data.size });
    zox_set(e, Anchor, { element_data.anchor });
    zox_set(e, Layer2D, { element_data.layer });
    zox_set(e, ParentLink, { parent_data.e });
    zox_set(e, CanvasLink, { canvas_data.e });
    if (canvas_data.e == parent_data.e) {
        on_child_added(world, canvas_data.e, e);
        zox_set(canvas_data.e, WindowToTop, { e });
    }

    /*element_data.position_in_canvas = get_element_pixel_positionv(
        parent_data.position,
        parent_data.size,
        element_data.position,
        element_data.anchor);

    const float2 real_position = get_element_position(
        element_data.position_in_canvas,
        canvas_data.size);*

    // anchor our position
    int2 position = element_data.position;
    anchor_element_position2(&position, element_data.anchor, element_data.size);

    zox_set(e, Position2, { real_position });
    zox_set(e, CanvasPosition, { element_data.position_in_canvas });

    */

    // zox_set(e, TextureSize, { element_data.size })
    /*if (element_data.render_disabled) {
        zox_set(e, RenderDisabled, { element_data.render_disabled })
    }*/
}
