// audio out through sdl's own audio (no sdl_mixer): a stereo float device that pulls from the engine's mixer
//  - sdl calls back on its audio thread, zox_mixer_render (sounds/mix) fills it
//  - the mixer guards its voices with zox_audio_lock / zox_audio_unlock
#define zox_audio_channels 2
void zox_mixer_render(float* out, int frames);  // sounds/mix/mixer.c

#ifndef zox_sdl3
    typedef SDL_mutex SDL_Mutex;
#endif
static SDL_Mutex* zox_audio_mutex = NULL;

void zox_audio_lock() {
    if (zox_audio_mutex) {
        SDL_LockMutex(zox_audio_mutex);
    }
}

void zox_audio_unlock() {
    if (zox_audio_mutex) {
        SDL_UnlockMutex(zox_audio_mutex);
    }
}

#ifdef zox_sdl3

static SDL_AudioStream* zox_audio_stream = NULL;

static void zox_audio_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount) {
    (void) userdata;
    (void) total_amount;
    static float buffer[4096 * zox_audio_channels];
    const int frame_bytes = sizeof(float) * zox_audio_channels;
    int frames = additional_amount / frame_bytes;
    while (frames > 0) {
        const int count = frames < 4096 ? frames : 4096;
        zox_mixer_render(buffer, count);
        SDL_PutAudioStreamData(stream, buffer, count * frame_bytes);
        frames -= count;
    }
}

byte initialize_sdl_audio(uint sample_rate) {
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        zox_loge("sdl audio could not initialize [%s]", SDL_GetError());
        return EXIT_FAILURE;
    }
    zox_audio_mutex = SDL_CreateMutex();
    const SDL_AudioSpec spec = {
        .format = SDL_AUDIO_F32,
        .channels = zox_audio_channels,
        .freq = (int) sample_rate
    };
    zox_audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, zox_audio_callback, NULL);
    if (!zox_audio_stream) {
        zox_loge("sdl audio device could not open [%s]", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_ResumeAudioStreamDevice(zox_audio_stream);
    return EXIT_SUCCESS;
}

void zox_dispose_audio() {
    if (zox_audio_stream) {
        SDL_DestroyAudioStream(zox_audio_stream);
        zox_audio_stream = NULL;
    }
    if (zox_audio_mutex) {
        SDL_DestroyMutex(zox_audio_mutex);
        zox_audio_mutex = NULL;
    }
}

#else

static SDL_AudioDeviceID zox_audio_device = 0;

static void zox_audio_callback(void* userdata, Uint8* stream, int length) {
    (void) userdata;
    zox_mixer_render((float*) stream, length / (sizeof(float) * zox_audio_channels));
}

byte initialize_sdl_audio(uint sample_rate) {
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        zox_loge("sdl audio could not initialize [%s]", SDL_GetError());
        return EXIT_FAILURE;
    }
    zox_audio_mutex = SDL_CreateMutex();
    SDL_AudioSpec want = {
        .freq = (int) sample_rate,
        .format = AUDIO_F32SYS,
        .channels = zox_audio_channels,
        .samples = 1024,
        .callback = zox_audio_callback,
    };
    // no allowed changes, sdl converts to the device
    zox_audio_device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);
    if (!zox_audio_device) {
        zox_loge("sdl audio device could not open [%s]", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_PauseAudioDevice(zox_audio_device, 0);
    return EXIT_SUCCESS;
}

void zox_dispose_audio() {
    if (zox_audio_device) {
        SDL_CloseAudioDevice(zox_audio_device);
        zox_audio_device = 0;
    }
    if (zox_audio_mutex) {
        SDL_DestroyMutex(zox_audio_mutex);
        zox_audio_mutex = NULL;
    }
}

#endif
