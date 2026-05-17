
void zox_spawn_prefabs_ui_containers(ecs* world) {
    if (prefab_slot) {
        zox_set(prefab_slot, DataLink, { 0 });
        zox_set(prefab_slot, DataDirty, { 0 });
    }
}
