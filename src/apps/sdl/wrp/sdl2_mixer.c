byte initialize_sdl_mixer(ulong sample_rate, byte channels_count) {
    if (Mix_OpenAudio(sample_rate, AUDIO_F32SYS, channels_count, 2048) < 0) {
        zox_loge("sdl audio could not initialize [%s]\n", Mix_GetError());
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}

int get_channels_count() {
    return Mix_GroupAvailable(-1);
}

static inline float* zox_mix_chunk_samples(Mix_Chunk *mix_chunk) {
    return (float*) mix_chunk->abuf;
}

static inline int zox_mix_chunk_length(Mix_Chunk *mix_chunk) {
    return mix_chunk->alen / sizeof(float);
}

static inline double get_mix_chunk_sound_length(Mix_Chunk* chunk) {
    int frequency, channels;
    Uint16 format;
    if (Mix_QuerySpec(&frequency, &format, &channels) == 0) {
        // Oh dear, SDL_mixer isn't initialized or is being bashful
        zox_log_error("failed finding sound frequency (mix length)");
        return -1.0;
    }
    // The size of a sample in bytes
    int bytes_per_sample = ((SDL_AUDIO_BITSIZE(format) / 8) * channels);
    // The total length in seconds
    return (double) chunk->alen / (frequency * bytes_per_sample);
}

static inline void zox_dispose_mixer() {
    Mix_Quit();
}