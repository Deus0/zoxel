entity spawn_prefab_streamer(ecs *world) {
    zox_prefab();
    zox_add_tag(e, Streamer);
    zox_prefab_set(e, StreamPoint, { int3_zero });
    zox_prefab_set(e, StreamPoint2, { int2_zero });
    zox_prefab_set(e, StreamDirty, { 0 });
    zox_prefab_set(e, StreamDirty2, { 0 });
    zox_prefab_set(e, BlockScale, { 0 });
    zox_prefab_set(e, NodeDepth, { 0 });
    return e;
}

entity spawn_streamer(ecs *world, entity prefab, int3 point){
    zox_instance(prefab)
    zox_set(e, StreamPoint, { point });
    zox_set(e, StreamPoint2, { { point.x, point.z } });
    return e;
}