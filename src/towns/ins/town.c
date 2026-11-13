// TODO: Add quadtree data and chunk2 tag
entity spawn_town(ecs* world, entity prefab, entity parent, lint seed, int2 position, byte2 size, byte wall_height, byte wall_thickness) {
    zox_instance(prefab);
    {
        char name[64];
        sprintf(name, "town_%ix%i", position.x, position.y);
        zox_name(name);
    }
    zox_set_parent(world, e, parent);
    zox_set(e, Seed, { seed });
    zox_set(e, BlockPosition2, { position });
    zox_set(e, TownSize, { size });
    zox_add(e, PlacementZone);   // a rect zone in its region (regions/fun/zones.c)
    zox_setv(e, ZoneSize, size);
    zox_set(e, WallHeight, { wall_height });
    zox_set(e, WallThickness, { wall_thickness });
    // its gate: a wall from its seed, a spot along it clear of its corners
    const byte side = seed_range(seed * 31, 0, 4);
    const int wall_half = (side & 1 ? size.y : size.x) / 2 - wall_thickness - 4;
    zox_setv(e, TownGateSide, side);
    zox_setv(e, TownGateAt, (byte) (128 + (wall_half > 0 ? seed_range(seed * 37, -wall_half, wall_half + 1) : 0)));
    // zox_set(e, RegionPosition, { position });
    // zox_set(e, RenderDistance, { rdistance });
    // zox_log("+ Tunk2 [%ix%i]", position.x, position.y);
    return e;
}
