// Basically Layout2??
entity spawn_ui(
    ecs *world,
    entity prefab,
    entity parent,
    float2 anchor,
    int2 position,
    int2 size,
    int2 texture_size)
{
    zox_instance(prefab);
    zox_name("ui");
    zox_setv(e, Anchor, anchor);
    zox_setv(e, LayoutPosition, position);
    zox_setv(e, LayoutSize, size);
    zox_setv(e, TextureSize, texture_size);
    if (parent) {
        zox_set_parent(world, e, parent);
    }
    return e;
}

entity spawn_uic(
    ecs* world,
    entity prefab,
    entity parent,
    float2 anchor,
    int2 position,
    int2 size,
    int2 texture_size,
    color fill,
    color outline)
{
    entity e = spawn_ui(
        world,
        prefab,
        parent,
        anchor,
        position,
        size,
        texture_size);
    zox_setv(e, FillColor, fill);
    zox_setv(e, OutlineColor, outline);
    return e;
}
