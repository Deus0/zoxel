entity spawn_image(
    ecs* world,
    entity p,
    entity texture,
    entity canvas,
    entity parent,
    int2 position,
    int2 size,
    float2 panchor,
    byte layer,
    byte rdisabled
) {

    zox_instance(p);
    zox_name("image");

    // TextureCloneSystem?
    // zox_set(e, TextureSize, { size });

    zox_set(e, LayoutPosition, { position });
    zox_set(e, LayoutSize, { size });
    zox_set(e, Anchor, { panchor });

    zox_set(e, Layer2D, { layer });
    zox_set(e, ParentLink, { parent });
    zox_set(e, CanvasLink, { canvas });

    if (canvas == parent) {
        on_child_added(world, canvas, e);
        zox_set(canvas, WindowToTop, { e });
    }

    zox_set(e, RenderDisabled, { rdisabled });

    // char* icon_texture_name = hook.texture_name;
    // clone_texture_to_entity(world, e, "paused");
    clone_texture_data(world, e, texture);

    return e;
}
