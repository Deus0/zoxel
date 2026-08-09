static SDL_AudioStream *audio_stream = NULL;

byte sdl_audio_play_sound(const float *data, int length, byte channels) {
    if (!data || length <= 0) {
        return 0;
    }

    if (!audio_stream) {
        SDL_AudioSpec want = {
            .freq = 48000,
            .format = SDL_AUDIO_F32,
            .channels = channels,
        };

        audio_stream = SDL_OpenAudioDeviceStream(
            SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
            &want,
            NULL,
            NULL
        );

        if (!audio_stream) {
            zox_loge("SDL_OpenAudioDeviceStream: %s", SDL_GetError());
            return 0;
        }

        if (!SDL_ResumeAudioStreamDevice(audio_stream)) {
            zox_loge("SDL_ResumeAudioStreamDevice: %s", SDL_GetError());
            SDL_DestroyAudioStream(audio_stream);
            audio_stream = NULL;
            return 0;
        }
    }
    if (!SDL_PutAudioStreamData(
        audio_stream,
        data,
        length * sizeof(float)
    )) {
        zox_log_error("SDL_PutAudioStreamData: %s", SDL_GetError());
        return 0;
    }
    return 1;
}