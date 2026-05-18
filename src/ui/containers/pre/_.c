void zox_spawn_prefabs_ui_containers(ecs* world) {
    if (prefab_slot) {
        zox_set(prefab_slot, DataLink, { 0 });
        zox_set(prefab_slot, DataDirty, { 0 });
    }
    if (prefab_icon_mouse_follow) {
        zox_prefab_set(prefab_icon_mouse_follow, DataLink, { 0 });
    }
}
