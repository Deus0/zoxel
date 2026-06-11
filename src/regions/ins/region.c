// TODO: Add quadtree data and chunk2 tag
entity spawn_region(ecs* world, entity prefab, entity parent, lint seed, int2 position, int2 region_size) {
    zox_instance(prefab);
    zox_set_parent(world, e, parent);
    {
        char name[64];
        sprintf(name, "region_%ix%i", position.x, position.y);
        zox_name(name);
    }
    zox_set(e, Seed, { seed });
    zox_set(e, BlockPosition2, { (int2) { position.x * region_size.x, position.y * region_size.y } });
    zox_set(e, BlockSize2, { region_size });
    // zox_set(e, RenderDistance, { rdistance });
    // zox_log("+ Tunk2 [%ix%i]", position.x, position.y);
    return e;
}
