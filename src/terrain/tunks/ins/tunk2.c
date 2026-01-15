// TODO: Add quadtree data and chunk2 tag
entity spawn_tunk2(ecs *world, entity prefab, entity terrain, int2 position, byte rdistance) {
    // entity e = spawn_chunk2(world, prefab, position);
    zox_instance(prefab);
    zox_name("tunk2");
    zox_set(e, VoxLink, { terrain });
    zox_set(e, Chunk2Position, { position });
    zox_set(e, RenderDistance, { rdistance });

    // zox_log("+ Tunk2 [%ix%i]", position.x, position.y);
    return e;
}
