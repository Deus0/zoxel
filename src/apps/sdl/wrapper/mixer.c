// this is our sdl mixer wrapper!
// todo: move to sdl module

byte use_sdl_audio = 0;
static SDL_AudioDeviceID audio_device = 0;

byte sdl_audio_play_sound(const float *data, int length, byte channels) {
    if (!data || length <= 0) {
        return 0;
    }
    if (!audio_device) {
        SDL_AudioSpec want = {
            .freq = 48000,
            .format = AUDIO_F32SYS,
            .channels = channels,
            .samples = 1024,
        };
        audio_device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);
        if (!audio_device)
        {
            zox_log_error("SDL_OpenAudioDevice: %s", SDL_GetError());
            return 0;
        }
        SDL_PauseAudioDevice(audio_device, 0);
    }
    if (SDL_QueueAudio(audio_device, data, length * sizeof(float)) < 0) {
        zox_log_error("SDL_QueueAudio: %s", SDL_GetError());
        return 0;
    }
    return 1;
}

#ifdef zox_sdl_mixer

byte initialize_sdl_mixer(ulong sample_rate, byte channels_count) {
    if (Mix_OpenAudio(sample_rate, AUDIO_F32SYS, channels_count, 2048) < 0) {
        zox_log_error("sdl audio could not initialize [%s]\n", Mix_GetError());
        return EXIT_FAILURE;
    } else {
        // Mix_ChannelFinished(free_mix_chunk);
        return EXIT_SUCCESS;
    }
}

int get_channels_count() {
    return Mix_GroupAvailable(-1);
}

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
        zox_log_error("failed finding sound frequency (mix length)")
        return -1.0;
    }
    // The size of a sample in bytes
    int bytes_per_sample = ((SDL_AUDIO_BITSIZE(format) / 8) * channels);
    // The total length in seconds
    return (double) chunk->alen / (frequency * bytes_per_sample);
}

#else

byte initialize_sdl_mixer(ulong sample_rate, byte channels_coun) {
    return EXIT_FAILURE;
}

int get_channels_count() {
    return -1;
}

void spawn_mixer_pool(int max_buffers, int buffer_size) { }

void dispose_mixer_pool() { }

#endif
