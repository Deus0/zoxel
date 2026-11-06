
#include "inside_block.c"

void zox_events_terrain_collisions(ecs* world) {
    zox_on_add(
        terrain_callback_inside_block,
        [in] blocks.InsideBlock,
        [in] transforms3.Position3D,
        [out] blocks.InsideBlockTime,
        [none] blocks.BlockSoundTrigger,
        [none] blocks.InsideBlockUpdate,
    );
}
