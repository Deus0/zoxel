entity spawn_prefab_sound(ecs *world) {
    zox_prefab();
    zox_prefab_name("sound");
    zox_add_tag(e, Sound);
    zox_prefab_set(e, SoundLength, { 0 });
    zox_prefab_set(e, SoundFrequency, { 440 });
    zox_prefab_set(e, SoundVolume, { default_sound_volume });
    zox_prefab_set(e, TriggerSound, { zox_dirty_none });
    zox_prefab_set(e, ProcessSound, { zox_dirty_none });
    zox_prefab_set(e, DestroyInTime, { sound_play_death_delay });
    zox_prefab_set(e, SoundData, { 0 });
#if defined(zox_sdl_mixer) && !defined(zox_sdl3)
    zox_prefab_set(e, SDLMixChunk, { NULL });
#endif

    return e;
}
