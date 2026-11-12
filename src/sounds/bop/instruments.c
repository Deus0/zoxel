// The instruments, their sound functions and envelopes, generated into samples
//  - the original ones share the generate settings (sound_attack_multiplier etc), a note that
//      would clip is scaled down
//  - the ambient ones have their own: struck / plucked ones a quick attack (they decay themselves),
//      bowed / sung ones a slow attack, and they're normalized to the same loudness
typedef float (*instrument_function)(float time, float frequency);

typedef struct {
    const char* name;
    instrument_function sound;
    float attack;       // seconds (0: the shared sound_attack_multiplier, of the length)
    float release;      // of the length, fades out at the end (0: the shared dampen)
    byte normalize;
} InstrumentData;

const InstrumentData instruments[instrument_end] = {
    [instrument_piano] = { "piano", piano_sound },
    [instrument_piano_square] = { "piano_square", piano_square_sound },
    [instrument_organ] = { "organ", organ_sound },
    [instrument_edm] = { "edm", edm_sound },
    [instrument_guitar] = { "guitar", guitar_sound },
    [instrument_flute] = { "flute", flute_sound },
    [instrument_violin] = { "violin", violin_sound },
    [instrument_saxophone] = { "saxophone", saxophone_sound },
    [instrument_trumpet] = { "trumpet", trumpet_sound },
    [instrument_unique] = { "unique", unique_sound },
    [instrument_soft_piano] = { "soft_piano", soft_piano_sound, 0.004f, 0.1f, 1 },
    [instrument_harp] = { "harp", harp_sound, 0.002f, 0.1f, 1 },
    [instrument_music_box] = { "music_box", music_box_sound, 0.002f, 0.1f, 1 },
    [instrument_cello] = { "cello", cello_sound, 0.12f, 0.25f, 1 },
    [instrument_choir] = { "choir", choir_sound, 0.3f, 0.3f, 1 },
    [instrument_bell] = { "bell", bell_sound, 0.003f, 0.1f, 1 },
};

static inline byte is_instrument(byte instrument) {
    return instrument > instrument_none && instrument < instrument_end && instruments[instrument].sound;
}

// fills samples (mono, sound_sample_rate) with a note, returns 0 for an unknown instrument
byte generate_instrument_samples(
    byte instrument,
    float frequency,
    float seconds,
    float volume,
    float noise,
    float* samples,
    uint length)
{
    if (!is_instrument(instrument) || !samples || !length || seconds <= 0) {
        return 0;
    }
    const InstrumentData data = instruments[instrument];
    if (!data.attack) {
        // the original instruments, as they always sounded
        const float attack = sound_attack_multiplier * seconds;
        const float dampen = sound_dampen_multiplier * seconds;
        float peak = 0;
        for (uint j = 0; j < length; j++) {
            const float time = (float) (j / sample_rate_f);
            float value = data.sound(time, frequency);
            if (noise) {
                value += noise * ((rand() / (float) RAND_MAX) * 2.0f - 1.0f);
            }
            value *= envelope(time, seconds, attack, dampen) * volume;
            samples[j] = value;
            peak = fmaxf(peak, fabsf(value));
        }
        // some went past full scale and were cut off (saxophone on a fifth of its samples),
        //  a note that would clip is scaled down instead, the rest are unchanged
        const float limit = 0.95f;
        const float scale = peak > limit ? limit / peak : 1;
        for (uint j = 0; j < length; j++) {
            samples[j] = clampf(samples[j] * scale, -1, 1);
        }
        return 1;
    }
    // ambient: shaped by its own attack and release, then normalized
    const float attack = fminf(data.attack, seconds * 0.25f);
    const float release_start = seconds * (1 - data.release);
    float peak = 0;
    for (uint j = 0; j < length; j++) {
        const float time = (float) (j / sample_rate_f);
        float value = data.sound(time, frequency);
        if (time < attack) {
            value *= time / attack;
        } else if (time > release_start) {
            const float t = 1 - (time - release_start) / (seconds - release_start);
            value *= t * t * (3 - 2 * t);
        }
        samples[j] = value;
        peak = fmaxf(peak, fabsf(value));
    }
    const float scale = peak > 0 ? 0.9f * volume / peak : 0;
    for (uint j = 0; j < length; j++) {
        samples[j] = clampf(samples[j] * scale, -1, 1);
    }
    return 1;
}
