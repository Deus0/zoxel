#include "drops.c"
#include "activate.c"
#include "consume.c"
#include "character.c"
#include "death.c"
#include "state.c"

void zox_systems_items(ecs* world) {
    zox_system_1(
        character_item_drop_system,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [none] core.DeathDirty,
        [none] characters.Character
    );
    zox_system_1(
        terrain_item_drop_system,
        zoxp_spawn,
        [in] chunks3.VoxelNodeQueue,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
        [none] chunks3.Chunk3
    );
    zox_system_1(
        character_items_spawn_system,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [none] characters.Character,
    );
    // NOTE: Timing issues due to Queue Clearing
    zox_system_1(
        item_activate_system,
        zoxp_spawn,
        [in] timers.Activate,
        [out] items.Quantity,
        [none] ItemBlock
    );
    zox_system_1(
        item_consume_system,
        zoxp_spawn,
        [in] timers.Activate,
        [in] items.ConsumeValue,
        [out] items.Quantity,
        [none] ItemConsumable
    );
    // States
    zox_system(
        quantity_dirty_system,
        zoxp_remove,
        [none] items.QuantityDirty
    );
}
