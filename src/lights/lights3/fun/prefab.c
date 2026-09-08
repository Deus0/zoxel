void prefab_add_lights3(ecs* world, entity e, byte depth) {
    zox_set(e, LightNode, { .value = darklight });
    zox_setv(e, LightNodeDepth, depth);
    // Queues
    zox_setv(e, SunlightQueue, 0);
    zox_setv(e, LightQueue, 0);
    zox_setv(e, DarkQueue, 0);
    // Events
    zox_setv(e, LightNodeDirty, 0);
    zox_setv(e, LightLock, 0);
}

byte is_chunk_lights_busy(ecs* world, entity e) {
    return zox_getv(e, LightNodeDirty);
}

byte is_chunk_generating_sunlights(ecs* world, entity e) {
    return zox_has(e, GenerateSunlight);
}
