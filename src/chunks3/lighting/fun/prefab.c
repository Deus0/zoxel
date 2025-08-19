void prefab_add_lights3(
    ecs* world,
    entity e,
    byte depth
) {
    zox_prefab_set(e, LightNode, { .value = darklight });
    zox_prefab_set(e, LightNodeDepth, { depth });
    zox_prefab_set(e, LightNodeDirty, { zox_dirty_none });
    zox_prefab_set(e, SunlightDirty, { zox_dirty_none });
    zox_prefab_set(e, LightQueue, { 0 });
    zox_prefab_set(e, DarkQueue, { 0 });

}