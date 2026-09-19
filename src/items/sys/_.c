#include "drops.c"
#include "activate.c"
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
        CharacterItemsSpawnSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [none] characters.Character,
    );
    // NOTE: Timing issues due to Queue Clearing
    zox_system_1(
        ItemActivateSystem,
        zoxp_spawn,
        [in] timers.Activate,
        [in] blocks.BlockLink,
        [out] items.Quantity,
        [none] ItemBlock
    );
    // States
    zox_system(
        quantity_dirty_system,
        zoxp_remove,
        [none] items.QuantityDirty
    );
}
