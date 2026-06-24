// TODO: Add quadtree data and chunk2 tag
entity spawn_region(ecs* world, entity prefab, entity parent, lint seed, int2 position, int2 size, int2 block_position, int2 block_size) {
    zox_instance(prefab);
    zox_set_parent(world, e, parent);
    {
        char name[64];
        sprintf(name, "region_%ix%i", position.x, position.y);
        zox_name(name);
    }
    zox_set(e, Seed, { seed });
    zox_set(e, RegionPosition, { position });
    zox_set(e, RegionSize, { size });
    zox_set(e, BlockPosition2, { block_position });
    zox_set(e, BlockSize2, { block_size });
    // zox_set(e, RenderDistance, { rdistance });
    // zox_log("+ Tunk2 [%ix%i]", position.x, position.y);
    return e;
}
