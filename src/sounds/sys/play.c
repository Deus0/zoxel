byte sdl_play_sound(SDLMixChunk* chunk, const float* data, int length, float volume, int channel) {
    if (use_sdl_audio) {
        return sdl_audio_play_sound(data, length, 2);
    }
    if (!length || !volume || !data) {
        return 0;
    }
    if (!chunk->value) {
        chunk->value = malloc(sizeof(Mix_Chunk));
    }
    if (!chunk->value) {
        return 0;
    }
    *chunk->value = (Mix_Chunk) {
        .allocated = 0,
        .volume = (Uint8)(volume * 128.f),
        .alen = length * sizeof(float),
        .abuf = (void*) data,
    };
    if (Mix_PlayChannel(channel, chunk->value, 0) == -1) {
        zox_loge("Failed to play sound: %s", Mix_GetError());
        return 0;
    }
    return 1;
}

zox_sys2(SoundPlaySystem) {
    int channel = get_channels_count();
    if (!audio_enabled || channel < 0 || nosounds) {
        return;
    }
    if (zox_current_time <= 1) {
        // zox_log("waiting: %f", zox_current_time);
        return;
    }
    zox_sys_begin()
    zox_sys_in(TriggerSound);
    zox_sys_in(SoundData);
    zox_sys_out(SDLMixChunk);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TriggerSound, triggerSound);
        zox_sys_i(SoundData, data);
        zox_sys_o(SDLMixChunk, chunk);
        if (triggerSound->value == zox_dirty_active) {
            if (data->value) {
                zox_logv("+ playing [%s] (%i:%f)", zox_sys_e_name, channel, mixer_audio_volume);
                sdl_play_sound(chunk, data->value, data->length, mixer_audio_volume, channel);
            } else {
                zox_log_error("Invalid SoundData, triggered.");
            }
        }
    }
} zox_sys_end(SoundPlaySystem);


zox_sys2(SoundPlayRefSystem) {
    int channel = get_channels_count();
    if (!audio_enabled || channel < 0 || nosounds) {
        return;
    }
    if (zox_current_time <= 1) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(TriggerSound);
    zox_sys_in(SoundDataRef);
    zox_sys_out(SDLMixChunk);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TriggerSound, triggerSound);
        zox_sys_i(SoundDataRef, soundDataRef);
        zox_sys_o(SDLMixChunk, chunk);
        if (triggerSound->value == zox_dirty_active) {
            if (soundDataRef->value) {
                sdl_play_sound(chunk, soundDataRef->value, soundDataRef->length, mixer_audio_volume, channel);
            }
        }
    }
} zox_sys_end(SoundPlayRefSystem);
