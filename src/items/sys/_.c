#include "item_drop.c"
#include "realm.c"
#include "activate.c"
zox_increment_system_with_reset(ItemDirty, zox_dirty_end);
zox_increment_system_with_reset(QuantityDirty, zox_dirty_end);
zox_declare_system_state_event(RealmItems, GenerateRealm, zox_generate_realm_items, spawn_realm_items)

void define_systems_items(ecs *world) {
    zoxd_system_increment(ItemDirty);
    zoxd_system_increment(QuantityDirty);
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
}