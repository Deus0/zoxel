#include "item_drop.c"
#include "realm.c"
#include "activate.c"
#include "terrain_drop.c"
#include "character.c"
zox_declare_system_state_event(RealmItems, GenerateRealm, zox_generate_realm_items, spawn_realm_items)

void define_systems_items(ecs *world) {
    zox_define_system_state_event_1(RealmItems, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm);
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
        [in] items.ItemLinks
    );
    zox_system_1(
        TerrainItemDropSystem,
        EcsOnUpdate,
        [in] chunks3.VoxelNodeQueue,
        [in] chunks3.VoxelNode,
        [in] chunks3.VoxLink,
        [in] chunks3.NodeDepth,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
    );
}