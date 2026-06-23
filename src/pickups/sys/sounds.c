zox_sys2(PickupSoundSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PickedUp);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(PickedUp, pickedUp);
        if (pickedUp->value == zox_dirty_active) {
            spawn_sound_from_file_name(
                world,
                prefab_sound,
                "pickup2",
                0,
                get_volume_sfx()
            );
        }
    }
} zox_sys_end(PickupSoundSystem);