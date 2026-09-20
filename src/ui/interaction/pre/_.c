void zox_prefabs_interaction(ecs *world) {
    // linking
    if (prefab_player) {
        zox_prefab_set(prefab_player, ClickingEntity, { 0 });
    }
    if (prefab_device) {
        zox_prefab_set(prefab_device, ClickingEntity, { 0 });
    }
    if (prefab_zevice_pointer) {
        zox_prefab_set(prefab_zevice_pointer, ClickingEntity, { 0 });
    }
}
