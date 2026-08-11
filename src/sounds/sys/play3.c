byte sdl_play_sound(const float* data, int length, float volume, int channel) {
    if (use_sdl_audio) {
        return sdl_audio_play_sound(data, length, 2);
    }
    if (!length || !volume || !data) {
        return 0;
    }
    SDL_AudioSpec spec = {
        .format = SDL_AUDIO_F32,
        .channels = 2,
        .freq = 44100
    };
    MIX_Audio* audio = MIX_LoadRawAudio(
        zox_mixer,
        data,
        length * sizeof(float),
        &spec
    );
    if (!audio) {
        zox_loge("Failed to load sound: %s", SDL_GetError());
        return 0;
    }
    if (!MIX_PlayAudio(zox_mixer, audio)) {
        zox_loge("Failed to play sound: %s", SDL_GetError());
        MIX_DestroyAudio(audio);
        return 0;
    }
    MIX_DestroyAudio(audio);
    return 1;
}

zox_sys2(SoundPlaySystem) {
    byte dbg_log = 0;
    byte channel = get_channels_count();
    if (!audio_enabled || channel < 0 || nosounds) {
        if (dbg_log) {
            zox_logw("Sound is disabled in [SoundPlaySystem], channel [%i]", channel);
        }
        return;
    }
    if (zox_current_time <= 1) {
        return;
    }
    zox_sys_begin()
    zox_sys_in(TriggerSound);
    zox_sys_in(SoundData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TriggerSound, triggerSound);
        zox_sys_i(SoundData, data);
        if (triggerSound->value != zox_dirty_active) {
            continue;
        }
        if (!data->value) {
            zox_loge("Invalid SoundData, triggered.");
            continue;
        }
        if (dbg_log) {
            zox_log("+ playing [%s] (%i:%f)", zox_sys_e_name, channel, mixer_audio_volume);
        }
        sdl_play_sound(data->value, data->length, mixer_audio_volume, channel);
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
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TriggerSound, state);
        zox_sys_i(SoundDataRef, sound);
        if (state->value != zox_dirty_active) {
            continue;
        }
        if (!sound->value) {
            continue;
        }
        sdl_play_sound(sound->value, sound->length, mixer_audio_volume, channel);
    }
} zox_sys_end(SoundPlayRefSystem);
