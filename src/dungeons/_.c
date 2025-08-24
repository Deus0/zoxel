#ifndef zox_dungeons
#define zox_dungeons

#include "settings/_.c"
zox_tag(Dungeon);
zox_tag(DungeonCore);
zoxc_byte(DungeonWallType);
#include "prefabs/_.c"
#include "util/_.c"
#include "systems/_.c"

zox_begin_module(Dungeons)
    zoxd_tag(Dungeon);
    zoxd_tag(DungeonCore);
    zoxd_byte(DungeonWallType);
    // main thread as it spawns currently
    zox_system_1(DungeonBlockSystem,
        zoxp_queue_add,
        [in] timing.TimerState,
        [in] chunks3.ChunkLink,
        [in] DungeonWallType,
        [none] blocks.BlockDungeon
    );
    add_hook_spawn_blocks(&spawn_block_dungeon_blocks);
    add_hook_spawned_block(&spawn_world_dungeon_core);
zox_end_module(Dungeons)

#endif