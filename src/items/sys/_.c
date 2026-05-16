#include "item_drop.c"
#include "activate.c"
#include "terrain_drop.c"
#include "character.c"
#include "character_player.c"
realm_clear_system(ItemLinks);

void define_systems_items(ecs* world) {
    realm_clear_systemd(items, ItemLinks);
    zox_system_1(
        ItemActivateSystem,
        EcsOnUpdate,
        [in] users.Activate,
        [in] users.UserLink,
        [in] blocks.BlockLink,
        [out] users.Quantity,
        [out] items.QuantityDirty,
        [none] ItemBlock
    );
    zox_system_1(
        ItemDropSystem,
        EcsOnUpdate,
        [in] combat.Dead,
        [in] transforms3.Position3D,
        [none] characters.Character
    );
    zox_system_1(
        TerrainItemDropSystem,
        EcsOnUpdate,
        [in] chunks3.VoxelNodeQueue,
        [in] chunks3.VoxelNode,
        [in] voxes.VoxLink,
        [in] chunks.NodeDepth,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
    );
    zox_system_1(
        CharacterItemsSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [none] characters.Character,
        [none] !players.PlayerLink
    );
    zox_system_1(
        CharacterPlayerItemsSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        //[in] realms.RealmLink,
        [none] characters.Character,
        [none] players.PlayerLink
    );
}
