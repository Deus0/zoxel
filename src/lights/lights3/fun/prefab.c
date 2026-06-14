void prefab_add_lights3(ecs* world, entity e, byte depth) {
    zox_prefab_set(e, LightNode, { .value = darklight });
    zox_prefab_set(e, LightNodeDepth, { depth });
    // States
    zox_prefab_set(e, GenerateLights, { zox_dirty_none });
    zox_prefab_set(e, LightNodeDirty, { zox_dirty_none });
    // Queues
    zox_prefab_set(e, SunlightQueue, { 0 });
    zox_prefab_set(e, LightQueue, { 0 });
    zox_prefab_set(e, DarkQueue, { 0 });
}

byte is_chunk_lights_busy(ecs* world, entity e) {
    if (!zox_valid(e)) {
        return 0;
    }
    return zox_getv(e, GenerateLights) ||
        zox_getv(e, LightNodeDirty);
}