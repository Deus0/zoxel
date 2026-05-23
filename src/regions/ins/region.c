// TODO: Add quadtree data and chunk2 tag
entity spawn_region(ecs* world, entity prefab, entity terrain, int2 position) {
    zox_instance(prefab);
    zox_name("region");
    zox_set(e, VoxLink, { terrain });
    zox_set(e, RegionPosition, { position });
    // zox_set(e, RenderDistance, { rdistance });
    // zox_log("+ Tunk2 [%ix%i]", position.x, position.y);
    return e;
}
