// The mixer: sums the playing sounds (voices) into stereo, for sdl's audio device (apps/sdl/wrp/audio.c)
//  - zox_mixer_play copies a sound's samples (mono) into a free voice, with its left and right gains
//  - 3D voices keep their source position, their gains follow the listener every frame
//  - zox_mixer_render runs on the audio thread: adds up the voices, then a soft limiter (no harsh clipping)
//  - finished voices are freed on the main thread, when the next sound plays
//  - when every voice is busy, the one closest to finishing makes way
//  - a voice can wait before it starts (delay, in samples), so a jingle's notes are timed exactly
#define mixer_voices_max 64
#define mixer_limit_knee 0.8f

#ifndef zox_sdl
    // no audio device, nothing renders on another thread
    static inline void zox_audio_lock() { }
    static inline void zox_audio_unlock() { }
#endif

typedef struct {
    float* data;
    uint length;
    uint position;
    uint delay;         // samples to wait before it starts
    float left;
    float right;
    float3 source;
    byte spatial;
    byte active;
} MixerVoice;

static MixerVoice mixer_voices[mixer_voices_max];
float mixer_volume = 1;     // master gain of the mix

// the listener, from the entity tagged SoundListener (the game camera)
byte sound_listener_active = 0;
float3 sound_listener_position;
float4 sound_listener_rotation;

static inline SoundGains mixer_gains(const float3* source) {
    if (!source || !sound_listener_active) {
        return sound_gains_center;
    }
    return sound_spatial_gains(sound_listener_position, sound_listener_rotation, *source);
}

// frees voices that finished (main thread), call while locked, returns what to free after
static inline void mixer_collect(float** to_free, int* to_free_count) {
    for (int i = 0; i < mixer_voices_max; i++) {
        MixerVoice* voice = &mixer_voices[i];
        if (!voice->active && voice->data) {
            to_free[(*to_free_count)++] = voice->data;
            voice->data = NULL;
        }
    }
}

// plays mono samples after a delay (seconds), from a source position (3D) or NULL (in the middle),
//  returns the voice or -1
int zox_mixer_play_delayed(const float* data, uint length, const float3* source, float delay) {
    if (!data || !length) {
        return -1;
    }
    const SoundGains gains = mixer_gains(source);
    if (source && sound_gains_silent(gains)) {
        return -1;  // too far away to hear
    }
    float* copy = malloc(length * sizeof(float));
    if (!copy) {
        return -1;
    }
    memcpy(copy, data, length * sizeof(float));
    float* to_free[mixer_voices_max + 1];
    int to_free_count = 0;
    zox_audio_lock();
    mixer_collect(to_free, &to_free_count);
    int index = -1;
    uint least_left = UINT32_MAX;
    for (int i = 0; i < mixer_voices_max; i++) {
        const MixerVoice* voice = &mixer_voices[i];
        if (!voice->active) {
            index = i;
            break;
        }
        const uint left = voice->delay + voice->length - voice->position;
        if (left < least_left) {
            least_left = left;
            index = i;
        }
    }
    MixerVoice* voice = &mixer_voices[index];
    if (voice->data) {
        to_free[to_free_count++] = voice->data;
    }
    *voice = (MixerVoice) {
        .data = copy,
        .length = length,
        .delay = delay > 0 ? (uint) (delay * sound_sample_rate) : 0,
        .left = gains.left,
        .right = gains.right,
        .source = source ? *source : float3_zero,
        .spatial = source != NULL,
        .active = 1,
    };
    zox_audio_unlock();
    for (int i = 0; i < to_free_count; i++) {
        free(to_free[i]);
    }
    return index;
}

// plays mono samples now
int zox_mixer_play(const float* data, uint length, const float3* source) {
    return zox_mixer_play_delayed(data, length, source, 0);
}

// the listener moved or turned, 3D voices follow it
void zox_mixer_set_listener(float3 position, float4 rotation) {
    zox_audio_lock();
    sound_listener_active = 1;
    sound_listener_position = position;
    sound_listener_rotation = rotation;
    for (int i = 0; i < mixer_voices_max; i++) {
        MixerVoice* voice = &mixer_voices[i];
        if (voice->active && voice->spatial) {
            const SoundGains gains = sound_spatial_gains(position, rotation, voice->source);
            voice->left = gains.left;
            voice->right = gains.right;
        }
    }
    zox_audio_unlock();
}

static inline float mixer_limit(float value) {
    const float magnitude = fabsf(value);
    if (magnitude <= mixer_limit_knee) {
        return value;
    }
    const float over = (magnitude - mixer_limit_knee) / (1 - mixer_limit_knee);
    const float limited = mixer_limit_knee + (1 - mixer_limit_knee) * tanhf(over);
    return value < 0 ? -limited : limited;
}

// fills out with frames of stereo (interleaved), on the audio thread
void zox_mixer_render(float* out, int frames) {
    memset(out, 0, frames * 2 * sizeof(float));
    zox_audio_lock();
    for (int i = 0; i < mixer_voices_max; i++) {
        MixerVoice* voice = &mixer_voices[i];
        if (!voice->active) {
            continue;
        }
        // waiting to start
        uint offset = 0;
        if (voice->delay) {
            offset = voice->delay < (uint) frames ? voice->delay : (uint) frames;
            voice->delay -= offset;
            if (offset == (uint) frames) {
                continue;
            }
        }
        const uint left = voice->length - voice->position;
        const uint room = (uint) frames - offset;
        const uint count = room < left ? room : left;
        const float* samples = voice->data + voice->position;
        float* to = out + 2 * offset;
        for (uint j = 0; j < count; j++) {
            to[2 * j] += samples[j] * voice->left;
            to[2 * j + 1] += samples[j] * voice->right;
        }
        voice->position += count;
        if (voice->position >= voice->length) {
            voice->active = 0;
        }
    }
    zox_audio_unlock();
    for (int j = 0; j < frames * 2; j++) {
        out[j] = mixer_limit(out[j] * mixer_volume);
    }
}

int zox_mixer_voices_playing() {
    int count = 0;
    zox_audio_lock();
    for (int i = 0; i < mixer_voices_max; i++) {
        count += mixer_voices[i].active;
    }
    zox_audio_unlock();
    return count;
}

// stops every voice, i.e. tests start from silence
void zox_mixer_stop_all() {
    zox_audio_lock();
    for (int i = 0; i < mixer_voices_max; i++) {
        mixer_voices[i].active = 0;
    }
    zox_audio_unlock();
}

void dispose_mixer() {
    zox_audio_lock();
    for (int i = 0; i < mixer_voices_max; i++) {
        if (mixer_voices[i].data) {
            free(mixer_voices[i].data);
        }
        mixer_voices[i] = (MixerVoice) { 0 };
    }
    sound_listener_active = 0;
    zox_audio_unlock();
}
