static MIX_Mixer* zox_mixer;

byte initialize_sdl_mixer(ulong sample_rate, byte channels_count) {
    if (!MIX_Init()) {
        zox_loge("sdl mixer could not initialize [%s]\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_AudioSpec spec = {
        .format = SDL_AUDIO_F32,
        .channels = channels_count,
        .freq = sample_rate
    };
    zox_mixer = MIX_CreateMixerDevice(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
        &spec
    );
    if (!zox_mixer) {
        zox_loge("sdl mixer could not initialize [%s]\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static inline void zox_dispose_mixer() {
    MIX_Quit();
}

int get_channels_count() {
    return 1;
}

static inline float* zox_mix_chunk_samples(MIX_Audio *mix_audio) {
    return NULL;
}

static inline int zox_mix_chunk_length(MIX_Audio *mix_audio) {
    return 0;
}

static inline double get_mix_chunk_sound_length(MIX_Audio *mix_audio) {
    return -1.0;
}