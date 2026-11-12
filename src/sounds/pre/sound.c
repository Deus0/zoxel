entity spawn_prefab_sound(ecs *world) {
    zox_prefab();
    zox_prefab_name("sound");
    zox_add(e, Sound);
    zox_set(e, SoundLength, { 0 });
    zox_set(e, SoundFrequency, { 440 });
    zox_set(e, SoundVolume, { default_sound_volume });
    zox_set(e, TriggerSound, { zox_dirty_none });
    zox_set(e, ProcessSound, { zox_dirty_none });
    zox_set(e, DestroyInTime, { sound_play_death_delay });
    zox_set(e, SoundData, { 0 });

    return e;
}
