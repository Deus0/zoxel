

static SDL_AudioDeviceID audio_device = 0;

byte sdl_audio_play_sound(const float *data, int length, byte channels) {
    if (!data || length <= 0) {
        return 0;
    }
    if (!audio_device) {
        SDL_AudioSpec want = {
            .freq = (int) channel_sample_rate,  // the sounds' rate (they were played at 48000)
            .format = AUDIO_F32SYS,
            .channels = channels,
            .samples = 1024,
        };
        audio_device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);
        if (!audio_device)
        {
            zox_loge("SDL_OpenAudioDevice: %s", SDL_GetError());
            return 0;
        }
        SDL_PauseAudioDevice(audio_device, 0);
    }
    if (SDL_QueueAudio(audio_device, data, length * sizeof(float)) < 0) {
        zox_loge("SDL_QueueAudio: %s", SDL_GetError());
        return 0;
    }
    return 1;
}