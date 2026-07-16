// NOTE: Spawns a dungeon under regions
entity spawn_dungeon(ecs* world, entity prefab, entity parent, lint seed, int2 position, byte2 size) {
    zox_instance(prefab);
    {
        char name[64];
        sprintf(name, "dungeon_%ix%i", position.x, position.y);
        zox_name(name);
    }
    zox_set_parent(world, e, parent);
    zox_set(e, Seed, { seed });
    zox_set(e, BlockPosition2, { position });
    // zox_set(e, DungeonSize, { size });
    return e;
}
