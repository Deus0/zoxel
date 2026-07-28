byte sound_file_index = 0;

entity spawn_prefab_sound_file_instance(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("sound_file_instance");
    zox_prefab_set(e, ProcessSound, { zox_dirty_trigger });
    return e;
}

// clones the files sound data for processing to a new frequency
entity spawn_sound_from_file(ecs *world, entity prefab, entity src, float frequency, float volume) {
    if (!zox_valid(src)) {
        zox_loge("invalid sound_file [spawn_sound_from_file]");
        return 0;
    }
    if (!prefab) {
        zox_loge("invalid prefab [spawn_sound_from_file]");
        return 0;
    }
    zox_geter(src, SoundData, src_data);
    zox_geter_value(src, SoundLength, float, sound_length);
    if (!src_data->length) {
        zox_loge("invalid sdl_sound [spawn_sound_from_file]");
        return 0;
    }
    SoundData data = { 0 };
    data.value = soundpool_alloc(src_data->length);
    data.length = src_data->length;
    if (data.value == NULL) {
        zox_loge("Failed to [soundpool_alloc] inside [spawn_sound_from_file]");
        return 0;
    }
    memcpy(data.value, src_data->value, sizeof(float) * src_data->length);
    // ecs
    zox_instance(prefab);
    zox_name("sound_file_instance");
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
    return e;
}