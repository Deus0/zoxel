byte load_wav_data(const char* filepath, float** values, uint* samples, float* length, byte dbg_log) {
    SDL_AudioSpec spec;
    Uint8* wav_data = NULL;
    Uint32 wav_length = 0;

    if (!SDL_LoadWAV(filepath, &spec, &wav_data, &wav_length)) {
        zox_loge("sound file failed to load [%s] due to [%s]", filepath, SDL_GetError());
        return 0;
    }

    if (spec.freq <= 0 || spec.channels <= 0) {
        SDL_free(wav_data);
        *values = NULL;
        *samples = 0;
        *length = 0;
        return 0;
    }

    uint bytes_per_sample = SDL_AUDIO_BYTESIZE(spec.format);
    uint total_samples = wav_length / bytes_per_sample;
    uint frame_count = total_samples / spec.channels;

    *samples = total_samples;
    *length = (float) frame_count / (float) spec.freq;
    *values = malloc(sizeof(float) * total_samples);

    if (!*values) {
        SDL_free(wav_data);
        *samples = 0;
        *length = 0;
        return 0;
    }

    switch (spec.format) {
        case SDL_AUDIO_F32LE:
        case SDL_AUDIO_F32BE: {
            float* source = (float*) wav_data;
            for (uint i = 0; i < total_samples; i++) {
                (*values)[i] = source[i];
            }
            break;
        }
        case SDL_AUDIO_S16LE:
        case SDL_AUDIO_S16BE: {
            Sint16* source = (Sint16*) wav_data;
            for (uint i = 0; i < total_samples; i++) {
                (*values)[i] = (float) source[i] / 32768.0f;
            }
            break;
        }
        case SDL_AUDIO_S32LE:
        case SDL_AUDIO_S32BE: {
            Sint32* source = (Sint32*) wav_data;
            for (uint i = 0; i < total_samples; i++) {
                (*values)[i] = (float) source[i] / 2147483648.0f;
            }
            break;
        }
        case SDL_AUDIO_U8: {
            Uint8* source = (Uint8*) wav_data;
            for (uint i = 0; i < total_samples; i++) {
                (*values)[i] = ((float) source[i] - 128.0f) / 128.0f;
            }
            break;
        }
        case SDL_AUDIO_S8: {
            Sint8* source = (Sint8*) wav_data;
            for (uint i = 0; i < total_samples; i++) {
                (*values)[i] = (float) source[i] / 128.0f;
            }
            break;
        }
        default:
            zox_loge("Unsupported WAV format [%s] format [0x%x]", filepath, spec.format);
            free(*values);
            *values = NULL;
            *samples = 0;
            *length = 0;
            SDL_free(wav_data);
            return 0;
    }

    if (dbg_log) {
        zox_log("Loaded Wav [%s] Length [%u] Samples [%u] Channels [%i] Frequency [%i]",
            filepath, wav_length, *samples, spec.channels, spec.freq);
    }

    SDL_free(wav_data);
    return 1;
}