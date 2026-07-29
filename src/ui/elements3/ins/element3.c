// NOTE: Spawns an element3, like element2, but has Transforms3 underneath
entity spawn_element3(ecs *world, entity prefab, entity parent, float2 anchor, int2 position, int2 size) {
    zox_instance(prefab);
    zox_set_unique_name(e, "element3");
    zox_set_parent(world, e, parent);
    zox_setv(e, Anchor, anchor);
    zox_setv(e, LayoutPosition, position);
    zox_setv(e, LayoutSize, size);
    zox_setv(e, TextureSize, size);
    // zox_set(e, LocalPosition3D, { position });
    return e;
}
