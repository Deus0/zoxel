#include "item_drop.c"
#include "activate.c"
#include "terrain_drop.c"
#include "character.c"
#include "character_player.c"
#include "death.c"

void define_systems_items(ecs* world) {
    zox_system(
        ItemQuantityDeathSystem,
        zoxp_update,
        [in] items.QuantityDirty,
        [in] items.Quantity,
        [none] items.Item
    );
    zox_system_1(
        ItemDropSystem,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [none] core.DeathDirty,
        [none] characters.Character
    );
    zox_system_1(
        CharacterItemsSpawnSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [none] characters.Character,
        // [none] !players.PlayerLink
    );
    zox_system_1(
        CharacterPlayerItemsSystem,
        zoxp_spawn,
        [in] characters.GenerateCharacter,
        [none] characters.Character,
        [none] players.PlayerLink
    );
    // NOTE: Timing issues due to Queue Clearing
    zox_system_1(
        ItemActivateSystem,
        zoxp_spawn, // zoxp_queue_add,
        [in] timers.Activate,
        [in] blocks.BlockLink,
        [out] items.Quantity,
        [out] items.QuantityDirty,
        [none] ItemBlock
    );
    // TODO: Fix using a secondary queue for item drops
    zox_system_1(
        TerrainItemDropSystem,
        // zoxp_mainthread,
        // NOTE: as voxels gets added at zoxp_queue_pre_post_clear
        // zoxp_queue_pre_post_clear,
        zoxp_spawn,
        [in] chunks3.VoxelNodeQueue,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
        [none] chunks3.Chunk3
    );
}
