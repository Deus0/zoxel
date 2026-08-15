void prefab_add_streamer(ecs* world, entity e) {
    zox_add(e, Streamer);
    zox_prefab_set(e, StreamLink, { 0 });
    zox_prefab_set(e, StreamerLevel, { 0 });
    zox_prefab_set(e, StreamPosition, { int3_zero });
    zox_prefab_set(e, StreamDirty, { 0 });
    zox_prefab_set(e, StreamPosition2, { int2_zero });
    zox_prefab_set(e, StreamDirty2, { 0 });
    zox_prefab_set(e, BlockScale, { 0 });
    zox_prefab_set(e, NodeDepth, { 0 });
    // NOTE: Camera didnt have these initially before
}

entity spawn_prefab_streamer(ecs *world) {
    zox_prefab();
    prefab_add_streamer(world, e);
    zox_prefab_set(e, Position3D, { float3_zero });
    return e;
}
