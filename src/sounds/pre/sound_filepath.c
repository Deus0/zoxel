entity spawn_prefab_sound_filepath(ecs *world) {
    zox_prefab();
    zox_prefab_name("sound_filepath");
    zox_prefab_set(e, SoundVolume, { default_sound_volume });
    zox_prefab_add(e, SoundData);
    return e;
}

entity spawn_sound_filepath(ecs *world, entity prefab, const float* value, int length, float sound_length) {
    if (!prefab) {
        zox_loge("(spawn_sound_filepath) Invalid [prefab]")
        return 0;
    }
    if (!value) {
        zox_loge("(spawn_sound_filepath) Invalid Input [value]");
        return 0;
    }
    if (length > MAX_SOUND_DATA) {
        zox_loge("(spawn_sound_filepath) length out of bounds [%i] of [%i]", length, MAX_SOUND_DATA);
        length = MAX_SOUND_DATA;
    }
    SoundData data = { 0 };
    data.value = soundpool_alloc(length);
    data.length = length;
    if (!data.value) {
        zox_loge("(spawn_sound_filepath) Null Sound Allocation");
        return 0;
    }
    zox_instance(prefab);
    zox_set(e, SoundLength, { sound_length });
    memcpy(data.value, value, length * sizeof(float));
    zox_set_ptr(e, SoundData, data);
    return e;
}