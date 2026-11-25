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

// a chunk with lights: a chunk3 parent's (spawn_chunk3_parent_chunk), terrain's chunks build on it
entity prefab_chunk_lit;

void spawn_prefabs_lights3(ecs* world) {
    entity e = zox_prefab_from_parent(world, prefab_chunk_textured);
    zox_set_unique_name(e, "chunk_lit");
    prefab_add_lights3(world, e, 0);
    prefab_chunk_lit = e;
}

byte is_chunk_lights_busy(ecs* world, entity e) {
    return zox_getv(e, LightOctreeDirty);
}

byte is_chunk_generating_sunlights(ecs* world, entity e) {
    return zox_has(e, GenerateSunlight);
}
