entity spawn_prefab_header(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("header");
    zox_add(e, Header);
    // Texture
    zox_add(e, FixToLayout);
    zox_add(e, FrameTexture);
    zox_setv(e, FrameCorner, header_corner_size);
    zox_setv(e, OutlineThickness, header_outline_size);
    zox_setv(e, FillColor, header_fill);
    zox_setv(e, OutlineColor, header_outline);
    zox_setv(e, ElementFillColor, header_fill);
    zox_setv(e, ElementOutlineColor, header_outline);
    // Dragging
    zox_add(e, Dragable);
    zox_setv(e, DraggableState, 0);
    zox_setv(e, DraggingDelta, int2_zero);
    return e;
}
