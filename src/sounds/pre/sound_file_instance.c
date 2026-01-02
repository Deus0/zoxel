byte sound_file_index = 0;

entity spawn_prefab_sound_file_instance(ecs *world,
    const entity prefab)
{
    zox_prefab_child(prefab)
    zox_prefab_name("sound_file_instance")
    zox_prefab_set(e, ProcessSound, { zox_dirty_trigger })
    return e;
}

// clones the files sound data for processing to a new frequency
entity spawn_sound_from_file(ecs *world,
    const entity prefab,
    const entity src,
    float frequency,
    float volume)
{
    if (!zox_valid(src)) {
        zox_log_error("invalid sound_file [spawn_sound_from_file]");
        return 0;
    }
    if (!prefab) {
        zox_log_error("invalid prefab [spawn_sound_from_file]");
        return 0;
    }
    zox_geter(src, SoundData, src_data);
    zox_geter_value(src, SoundLength, float, sound_length);
    if (!src_data->length) {
        zox_log_error("invalid sdl_sound [spawn_sound_from_file]");
        return 0;
    }
    // ecs
    zox_instance(prefab)
    zox_name("sound_file_instance")

    SoundData data = { 0 };
    data.value = soundpool_alloc();
    data.length = src_data->length;
    memcpy(data.value, src_data->value, sizeof(float) * src_data->length);
    zox_set_ptr(e, SoundData, data);

    zox_set(e, SoundVolume, { default_sound_volume_loaded });
    zox_set(e, SoundLength, { sound_length });
    if (frequency) {
        zox_set(e, SoundFrequency, { frequency });
    }
    if (volume) {
        zox_set(e, SoundVolume, { volume });
    }
    zox_set(e, ProcessSound, { zox_dirty_trigger });
    // zox_set(e, DestroyInTime, { 1 });
    return e;
}