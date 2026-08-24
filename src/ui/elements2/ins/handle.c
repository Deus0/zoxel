color handle_outline = color_white;
color handle_fill = color_black;

entity spawn_handle(
    ecs* world,
    entity prefab,
    entity parent,
    int2 parent_size,
    int2 position,
    int2 size,
    float2 position_anchor,
    color fill,
    color outline,
    byte direction)
{
    zox_instance(prefab);
    zox_name("handle");
    zox_set_parent(world, e, parent);
    zox_set(e, LayoutPosition, { position });
    zox_set(e, LayoutSize, { size });
    zox_set(e, Anchor, { position_anchor });
    zox_set(e, FillColor, { fill });
    zox_set(e, OutlineColor, { outline });
    zox_set(e, ElementFillColor, { fill });
    zox_set(e, ElementOutlineColor, { outline });
    // Constrains within slider
    if (!direction) {
        int bounds = (parent_size.x / 2) - size.x / 2;
        zox_set(e, LayoutConstraints, { (int4) { -bounds, bounds, 0, 0 } });
    } else {
        int bounds = (parent_size.y / 2) - size.y / 2;
        zox_set(e, LayoutConstraints, { (int4) { 0, 0, -bounds, bounds } });
    }
    return e;
}
