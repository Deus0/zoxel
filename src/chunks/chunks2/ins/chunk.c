entity spawn_chunk2(ecs *world, entity prefab, int2 position) {
    zox_instance(prefab);
    zox_name("chunk2")
    zox_set(e, Chunk2Position, { position });
    return e;
}