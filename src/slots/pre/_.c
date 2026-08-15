entity prefab_slot;

void zox_spawn_prefabs_slots(ecs* world) {
    {
        prefab_slot = zox_new();
        zox_add(prefab_slot, Slot);
        zox_make_prefab(prefab_slot);
    }
}
