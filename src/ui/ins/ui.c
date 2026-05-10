// Basically Layout2??
entity spawn_ui(ecs *world, entity prefab, entity parent, float2 anchor, int2 position, int2 size, int2 texture_size) {
    zox_instance(prefab);
    zox_name("ui");
    zox_set(e, Anchor, { anchor });
    zox_set(e, LayoutPosition, { position });
    zox_set(e, LayoutSize, { size });
    zox_set(e, TextureSize, { texture_size });
    zox_set_parent(world, e, parent);
    entity canvas = zox_get_root_canvas(world, parent);
    if (zox_valid(canvas)) {
        zox_set(e, CanvasLink, { canvas });
    }
    // zox_set(e, CanvasLink, { canvas });
    // Where we link to canvas children
    // zox_set(e, ParentLink, { parent });
    // if (canvas == parent) zox_set(canvas, WindowToTop, { e });
    // zox_set(e, Layer2D, { layer });
    return e;
}

entity spawn_uic(ecs* world, entity prefab, entity parent, float2 anchor, int2 position, int2 size, int2 texture_size, color fill, color outline) {
    entity e = spawn_ui(world, prefab, parent, anchor, position, size, texture_size);
    zox_set(e, Color, { fill });
    zox_set(e, OutlineColor, { outline });
    return e;
}
