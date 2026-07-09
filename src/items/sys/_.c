#include "item_drop.c"
#include "activate.c"
#include "terrain_drop.c"
#include "character.c"
#include "character_player.c"
#include "death.c"
realm_clear_system(ItemLinks);

void define_systems_items(ecs* world) {
    realm_clear_systemd(items, ItemLinks);
    zox_system(
        ItemQuantityDeathSystem,
        EcsOnUpdate,
        [in] items.QuantityDirty,
        [in] items.Quantity,
        [none] items.Item
    );
    zox_system_1(
        ItemActivateSystem,
        zoxp_mainthread,
        [in] timers.Activate,
        [in] blocks.BlockLink,
        [out] items.Quantity,
        [out] items.QuantityDirty,
        [none] ItemBlock
    );
    zox_system_1(
        ItemDropSystem,
        zoxp_mainthread,
        [in] combat.Dead,
        [in] transforms3.Position3D,
        [none] characters.Character
    );
    zox_system_1(
        CharacterItemsSpawnSystem,
        zoxp_mainthread,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [none] characters.Character,
        // [none] !players.PlayerLink
    );
    zox_system_1(
        CharacterPlayerItemsSystem,
        zoxp_mainthread,
        [in] characters.GenerateCharacter,
        [none] characters.Character,
        [none] players.PlayerLink
    );
    // NOTE: Timing issues due to Queue Clearing
    zox_system_1(
        TerrainItemDropSystem,
        zoxp_queue_process,
        [in] chunks3.VoxelNodeQueue,
        [in] chunks3.VoxelNode,
        [in] chunks.NodeDepth,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
        [none] chunks3.Chunk3
    );
}
