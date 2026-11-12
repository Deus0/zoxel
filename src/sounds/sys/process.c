// todo: alter frequency over time during sound
void sound_process_system(iter* it) {
    zox_sys_on_begin();
    if (nosounds) {
        return;
    }
    float default_frequency = 220; // 440;
    zox_sys_begin();
    zox_sys_in(ProcessSound);
    zox_sys_in(SoundData);
    zox_sys_in(SoundVolume);
    zox_sys_in(SoundFrequency);
    zox_sys_out(TriggerSound);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ProcessSound, processSound);
        zox_sys_i(SoundData, soundData);
        zox_sys_i(SoundVolume, soundVolume);
        zox_sys_i(SoundFrequency, soundFrequency);
        zox_sys_o(TriggerSound, triggerSound);
        if (processSound->value != zox_dirty_active) {
            continue;
        }
        if (!soundData->value) {
            zox_loge("Failed processing, no sound data");
            continue;
        }
        float frequency = soundFrequency->value;
        if (frequency <= 0) {
            zox_loge("Failed processing, frequency [%f]", frequency);
            continue;
        }
        float frequency_scale = frequency / default_frequency;
        // heap, a few seconds of samples is too big for a worker thread's stack
        float* new_data = malloc(soundData->length * sizeof(float));
        if (!new_data) {
            zox_loge("Failed processing, out of memory");
            continue;
        }
        byte out_of_bounds = 0;
        for (uint j = 0; j < soundData->length; j++) {
            if (out_of_bounds) {
                new_data[j] = 0.0f;
                continue;
            }
            float source_index = j / frequency_scale;
            int index1 = (int) source_index;
            uint index2 = index1 + 1;
            // Ensure indices are within bounds
            if (index2 >= soundData->length) {
                out_of_bounds = 1;
                new_data[j] = 0.0f;
                continue;
            }
            float time = source_index - index1;
            // Linear interpolation between samples
            new_data[j] = soundData->value[index1] * (1.0f - time) + soundData->value[index2] * time;
            new_data[j] *= soundVolume->value;
        }
        memcpy(soundData->value, new_data, soundData->length * sizeof(float));
        free(new_data);
        triggerSound->value = zox_dirty_trigger;
        zox_logv("  -> updated sound frequency to [%f]", frequency_scale);
    }
    zox_sys_on_end();
} zoxd_system(sound_process_system);
