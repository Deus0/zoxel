#include "dungeon_core.c"

void define_systems_dungeons(ecs* world) {
    // main thread as it spawns currently
    zox_system_1(DungeonBlockSystem,
        zoxp_queue_add,
        [in] timing.TimerState,
        [in] chunks3.ChunkLink,
        [in] DungeonWallType,
        [none] blocks.BlockDungeon
    );
}