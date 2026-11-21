
#include "inside_block.c"
#include "water_splash.c"

void zox_events_terrain_collisions(ecs* world) {
    zox_on_add(
        terrain_callback_inside_block,
        [in] blocks.InsideBlock,
        [in] transforms3.Position3D,
        [out] blocks.InsideBlockTime,
        [none] blocks.BlockSoundTrigger,
        [none] blocks.InsideBlockDirty,
    );
    zox_on_add(
        water_splash_event,
        [in] blocks.InsideBlock,
        [in] blocks.InsideBlockPosition,
        [in] physics3.Velocity3D,
        [out] terrains.InsideBlockLast,
        [none] blocks.InsideBlockDirty,
    );
    // last: both above see it, its removal lands after them (deferred)
    zox_on_add(inside_block_cleanup_event, [none] blocks.InsideBlockDirty);
}
