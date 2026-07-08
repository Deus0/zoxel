void prefab_add_lights3(ecs* world, entity e, byte depth) {
    zox_prefab_set(e, LightNode, { .value = darklight });
    zox_prefab_set(e, LightNodeDepth, { depth });
    // States
    zox_prefab_set(e, GenerateLights, { 0 });
    zox_prefab_set(e, RefreshLights, { 0 });
    zox_prefab_set(e, LightNodeDirty, { zox_dirty_none });
    // Queues
    zox_prefab_set(e, SunlightQueue, { 0 });
    zox_prefab_set(e, LightQueue, { 0 });
    zox_prefab_set(e, DarkQueue, { 0 });
}

byte is_chunk_lights_busy(ecs* world, entity e) {
    return zox_getv(e, LightNodeDirty);
}

byte is_chunk_generating_sunlights(ecs* world, entity e) {
    return zox_getv(e, GenerateLights);
}