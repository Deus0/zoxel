void zox_spawn_prefabs_ui_containers(ecs* world) {
    if (prefab_slot) {
        zox_prefab_set(prefab_slot, DataLink, { 0 });
    }
}
