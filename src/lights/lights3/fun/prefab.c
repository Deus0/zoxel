void prefab_add_lights3(ecs* world, entity e, byte depth) {
    zox_set(e, LightOctree, { .value = darklight });
    zox_setv(e, LightOctreeDepth, depth);
    // Queues
    zox_setv(e, SunlightQueue, 0);
    zox_setv(e, LightQueue, 0);
    zox_setv(e, DarkQueue, 0);
    // Events
    zox_setv(e, LightOctreeDirty, 0);
    zox_setv(e, LightLock, 0);
}

byte is_chunk_lights_busy(ecs* world, entity e) {
    return zox_getv(e, LightOctreeDirty);
}

byte is_chunk_generating_sunlights(ecs* world, entity e) {
    return zox_has(e, GenerateSunlight);
}
