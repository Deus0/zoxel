// TODO: Add quadtree data and chunk2 tag
entity spawn_town(ecs* world, entity prefab, entity parent, int2 position, int2 size) {
    zox_instance(prefab);
    {
        char name[64];
        sprintf(name, "town_%ix%i", position.x, position.y);
        zox_name(name);
    }
    zox_set_parent(world, e, parent);
    zox_set(e, VoxelPosition, { position.x, 0, position.y });
    zox_set(e, VoxelSize, { size.x, 0, size.y });
    // zox_set(e, RegionPosition, { position });
    // zox_set(e, RenderDistance, { rdistance });
    // zox_log("+ Tunk2 [%ix%i]", position.x, position.y);
    return e;
}
