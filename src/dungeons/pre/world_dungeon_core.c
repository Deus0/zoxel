// prefab for our world block
entity spawn_block_world_dungeon(ecs *world, entity wall) {
    zox_prefab();
    zox_name("block_dungeon_core");
    zox_add(e, BlockDungeon);
    zox_add(e, DungeonCore);
    zox_set(e, TimerRate, { dungeon_grow_rate });
    zox_set(e, TimerState, { 0 });
    zox_set(e, TimerTime, { 0 });
    // zox_set(e, DungeonWallType, { zox_block_dark });
    zox_set(e, DungeonWallType, { wall });
    return e;
}
