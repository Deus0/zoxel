// TODO: Add quadtree data and chunk2 tag
entity spawn_town(ecs* world, entity prefab, entity parent, int2 position, byte2 size, byte wall_height, byte wall_thickness) {
    zox_instance(prefab);
    {
        char name[64];
        sprintf(name, "town_%ix%i", position.x, position.y);
        zox_name(name);
    }
    zox_set_parent(world, e, parent);
    zox_set(e, BlockPosition2, { position });
    zox_set(e, TownSize, { size });
    zox_set(e, WallHeight, { wall_height });
    zox_set(e, WallThickness, { wall_thickness });
    // zox_set(e, RegionPosition, { position });
    // zox_set(e, RenderDistance, { rdistance });
    // zox_log("+ Tunk2 [%ix%i]", position.x, position.y);
    return e;
}
