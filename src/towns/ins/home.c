// TODO: Add quadtree data and chunk2 tag
entity spawn_home(ecs* world, entity prefab, entity parent, lint seed, int2 position, byte2 size, byte home_height) {
    zox_instance(prefab);
    {
        char name[64];
        sprintf(name, "home_%ix%i", position.x, position.y);
        zox_name(name);
    }
    zox_set_parent(world, e, parent);
    zox_set(e, Seed, { seed });
    zox_set(e, BlockPosition2, { position });
    zox_set(e, HomeSize, { size });
    zox_set(e, HomeHeight, { home_height });
    // zox_set(e, WallThickness, { wall_thickness });
    return e;
}
