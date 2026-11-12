// Voices: an animal-crossing like babble, each syllable a quick sung sound, every character's voice its own
//  - a voice: its pitch (deep to tiny), formant size (a big or small throat), speed, breath and timbre,
//      some alien: a vibrato wobble, or a ring modulated buzz
//  - a syllable: a consonant's onset (a burst, a hiss, a hum or a glide) into a vowel, shaped by the
//      vowel's formants (mouth resonances), its pitch falling a little (rising for a question)
//  - additive: the voice's harmonics, each weighted by how close it is to the formants
//  - normalized like the ambient instruments, so voices are as loud as each other

typedef struct {
    float pitch;        // hz
    float formant;      // formant scale, under 1 a big creature, over 1 a small one
    float speed;        // syllable length scale
    float breath;       // noise mixed in
    float tilt;         // harmonic roll off, low is buzzy and bright, high is soft
    float wobble;       // vibrato depth (of pitch)
    float wobble_rate;  // hz
    float ring;         // ring modulation amount, an alien buzz
    float ring_rate;    // hz
} Voice;

#define voice_syllable_seconds 0.11f
#define voice_max_harmonics 48
#define voice_max_frequency 5500.0f

typedef enum {
    voice_type_deep,
    voice_type_normal,
    voice_type_high,
    voice_type_tiny,
    voice_type_count
} VoiceType;

static inline float voice_rand(uint* state) {
    *state = *state * 1103515245u + 12345u;
    return ((*state >> 8) & 0xffff) / 65535.0f;
}

static inline float voice_range(uint* state, float a, float b) {
    return a + (b - a) * voice_rand(state);
}

// a character's voice, from its seed (the same seed always sounds the same)
Voice voice_from_seed(uint seed) {
    uint rng = seed * 2654435761u + 1;
    voice_rand(&rng);
    Voice voice = { 0 };
    const VoiceType type = (VoiceType) ((uint) (voice_rand(&rng) * voice_type_count) % voice_type_count);
    switch (type) {
        case voice_type_deep:
            voice.pitch = voice_range(&rng, 80, 125);
            voice.formant = voice_range(&rng, 0.78f, 0.9f);
            voice.speed = voice_range(&rng, 1.05f, 1.25f);
            break;
        case voice_type_normal:
            voice.pitch = voice_range(&rng, 135, 220);
            voice.formant = voice_range(&rng, 0.95f, 1.08f);
            voice.speed = voice_range(&rng, 0.9f, 1.1f);
            break;
        case voice_type_high:
            voice.pitch = voice_range(&rng, 240, 340);
            voice.formant = voice_range(&rng, 1.1f, 1.25f);
            voice.speed = voice_range(&rng, 0.8f, 1.0f);
            break;
        default:
            voice.pitch = voice_range(&rng, 360, 480);
            voice.formant = voice_range(&rng, 1.25f, 1.4f);
            voice.speed = voice_range(&rng, 0.7f, 0.9f);
            break;
    }
    voice.breath = voice_range(&rng, 0, 0.12f);
    voice.tilt = voice_range(&rng, 0.8f, 1.4f);
    // a quarter wobble, a fifth buzz (aliens)
    if (voice_rand(&rng) < 0.25f) {
        voice.wobble = voice_range(&rng, 0.015f, 0.05f);
        voice.wobble_rate = voice_range(&rng, 5, 11);
    }
    if (voice_rand(&rng) < 0.2f) {
        voice.ring = voice_range(&rng, 0.25f, 0.5f);
        voice.ring_rate = voice_range(&rng, 40, 110);
    }
    return voice;
}

// formants (hz) and their strengths, of each vowel, for an adult sized throat
typedef struct {
    float frequency[3];
    float strength[3];
} VoiceFormants;

static const VoiceFormants voice_vowels[] = {
    { { 800, 1200, 2500 }, { 1.0f, 0.7f, 0.25f } },     // a
    { { 450, 1900, 2600 }, { 1.0f, 0.6f, 0.3f } },      // e
    { { 300, 2300, 3000 }, { 1.0f, 0.5f, 0.35f } },     // i
    { { 480, 820, 2500 }, { 1.0f, 0.8f, 0.15f } },      // o
    { { 330, 700, 2400 }, { 1.0f, 0.6f, 0.1f } },       // u
    { { 550, 1500, 2500 }, { 1.0f, 0.6f, 0.2f } },      // schwa (a consonant on its own)
};
static const VoiceFormants voice_nasal = { { 260, 1000, 2300 }, { 1.0f, 0.15f, 0.08f } };
static const VoiceFormants voice_glide_l = { { 360, 1100, 2700 }, { 1.0f, 0.5f, 0.2f } };
static const VoiceFormants voice_glide_r = { { 420, 1250, 1650 }, { 1.0f, 0.6f, 0.4f } };

static inline int voice_vowel_index(char vowel) {
    switch (vowel) {
        case 'a': return 0;
        case 'e': return 1;
        case 'i': case 'y': return 2;
        case 'o': return 3;
        case 'u': return 4;
        default: return 5;
    }
}

static inline byte voice_is_vowel(char c) {
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y';
}

typedef enum {
    voice_onset_none,
    voice_onset_plosive,    // p t k (b d g softer)
    voice_onset_fricative,  // s z f v h sh
    voice_onset_nasal,      // m n
    voice_onset_glide,      // l r w y
} VoiceOnset;

static inline VoiceOnset voice_onset_of(char c) {
    switch (c) {
        case 'p': case 't': case 'k': case 'c': case 'q': case 'b': case 'd': case 'g': return voice_onset_plosive;
        case 's': case 'z': case 'f': case 'v': case 'h': case 'x': case 'j': return voice_onset_fricative;
        case 'm': case 'n': return voice_onset_nasal;
        case 'l': case 'r': case 'w': return voice_onset_glide;
        default: return voice_onset_none;
    }
}

// how much a harmonic at frequency passes through the formants
static inline float voice_formant_weight(const VoiceFormants* formants, float scale, float frequency) {
    float weight = 0.04f;
    for (int f = 0; f < 3; f++) {
        const float center = formants->frequency[f] * scale;
        const float width = 70 + center * 0.09f;
        const float d = (frequency - center) / width;
        weight += formants->strength[f] * expf(-d * d);
    }
    return weight;
}

// deterministic noise (the voice's breath and consonants)
static inline float voice_noise(uint* state) {
    return voice_rand(state) * 2 - 1;
}

// the length of a syllable of a voice, in seconds
static inline float voice_syllable_length(const Voice* voice) {
    return voice_syllable_seconds * voice->speed;
}

// fills samples with a syllable: consonant (0 for none) into vowel, inflection in semitones (rising is positive)
//  returns the samples written
uint generate_voice_syllable(const Voice* voice, char consonant, char vowel, float inflection,
    float volume, uint seed, float* samples, uint max_length)
{
    const float seconds = voice_syllable_length(voice);
    uint length = (uint) (seconds * sound_sample_rate);
    if (length > max_length) {
        length = max_length;
    }
    if (!length || !samples) {
        return 0;
    }
    const VoiceOnset onset = voice_onset_of(consonant);
    const VoiceFormants* target = &voice_vowels[voice_vowel_index(vowel)];
    const VoiceFormants* start = target;
    if (onset == voice_onset_nasal) {
        start = &voice_nasal;
    } else if (onset == voice_onset_glide) {
        start = consonant == 'l' ? &voice_glide_l : consonant == 'r' ? &voice_glide_r : &voice_vowels[4];
    }
    const byte voiced_plosive = consonant == 'b' || consonant == 'd' || consonant == 'g';
    const byte sibilant = consonant == 's' || consonant == 'z' || consonant == 'x' || consonant == 'j';
    // the consonant's part, then the vowel
    const float onset_seconds =
        onset == voice_onset_plosive ? 0.012f :
        onset == voice_onset_fricative ? 0.035f :
        onset == voice_onset_nasal || onset == voice_onset_glide ? 0.04f : 0;
    uint rng = seed * 747796405u + 2891336453u;
    // each syllable a little higher or lower (the babble's melody)
    const float offset = (voice_rand(&rng) - 0.5f) * 3;
    const float base_pitch = voice->pitch * powf(2, offset / 12.0f);
    // harmonic weights at the start and the vowel (formants glide between them)
    const int harmonics = (int) fminf(voice_max_harmonics, voice_max_frequency / base_pitch);
    float weights_start[voice_max_harmonics + 1];
    float weights_vowel[voice_max_harmonics + 1];
    for (int n = 1; n <= harmonics; n++) {
        const float frequency = base_pitch * n;
        // the throat's roll off, lifted towards the upper formants (they tell the vowels apart)
        const float source = (1 + frequency / 700.0f) / powf((float) n, voice->tilt);
        weights_start[n] = source * voice_formant_weight(start, voice->formant, frequency);
        weights_vowel[n] = source * voice_formant_weight(target, voice->formant, frequency);
    }
    float phase = 0;
    float last_noise = 0;
    float peak = 0;
    const float release_start = seconds * 0.55f;
    for (uint j = 0; j < length; j++) {
        const float time = j / sample_rate_f;
        const float progress = time / seconds;
        // pitch: falls through the syllable (animalese), a question rises
        float pitch = base_pitch * powf(2, (inflection * progress + 1.2f * (1 - progress) - 0.6f) / 12.0f);
        if (voice->wobble) {
            pitch *= 1 + voice->wobble * sinf(2 * M_PI * voice->wobble_rate * time);
        }
        phase += 2 * M_PI * pitch / sound_sample_rate;
        if (phase > 2 * M_PI * 64) {
            phase -= 2 * M_PI * 64;
        }
        // the formants glide from the consonant into the vowel
        const float glide = onset_seconds > 0 ? fminf(1, time / (onset_seconds + 0.02f)) : 1;
        float voiced = 0;
        for (int n = 1; n <= harmonics; n++) {
            const float weight = weights_start[n] + (weights_vowel[n] - weights_start[n]) * glide;
            voiced += weight * sinf(n * phase);
        }
        // a plosive is silent, bursts, then voices, a fricative hisses into the vowel
        float voicing = 1;
        float noise_amount = voice->breath;
        if (onset == voice_onset_plosive) {
            voicing = time < onset_seconds ? (voiced_plosive ? 0.3f : 0) : fminf(1, (time - onset_seconds) / 0.015f);
            if (time < onset_seconds) {
                noise_amount = voiced_plosive ? 0.5f : 1.2f;
            }
        } else if (onset == voice_onset_fricative && time < onset_seconds + 0.01f) {
            voicing = consonant == 'z' || consonant == 'v' || consonant == 'j' ? 0.35f : fminf(1, time / onset_seconds) * 0.25f;
            noise_amount = consonant == 'h' ? 0.35f : 0.8f;
        } else if (onset == voice_onset_nasal && time < onset_seconds) {
            voicing = 0.7f;
        }
        // sibilants are bright noise (its difference, a crude high pass), others plain
        const float raw_noise = voice_noise(&rng);
        const float noise = sibilant ? (raw_noise - last_noise) * 0.7f : raw_noise * 0.5f;
        last_noise = raw_noise;
        float value = voiced * voicing + noise * noise_amount;
        if (voice->ring) {
            value *= 1 - voice->ring + voice->ring * sinf(2 * M_PI * voice->ring_rate * time);
        }
        // a quick attack, a smooth release
        float envelope = fminf(1, time / 0.006f);
        if (time > release_start) {
            const float t = 1 - (time - release_start) / (seconds - release_start);
            envelope *= t * t * (3 - 2 * t);
        }
        value *= envelope;
        samples[j] = value;
        peak = fmaxf(peak, fabsf(value));
    }
    const float scale = peak > 0 ? 0.9f * volume / peak : 0;
    for (uint j = 0; j < length; j++) {
        samples[j] = clampf(samples[j] * scale, -1, 1);
    }
    return length;
}

// says a syllable from a source (3D) or in the middle (0)
entity spawn_voice_syllable(ecs* world, const Voice* voice, char consonant, char vowel, float inflection, float volume, entity source) {
    if (nosounds || !master_volume) {
        return 0;
    }
    const uint max_length = (uint) (voice_syllable_length(voice) * sound_sample_rate) + 1;
    SoundData data = { 0 };
    data.value = soundpool_alloc(max_length);
    if (!data.value) {
        return 0;
    }
    data.length = generate_voice_syllable(voice, consonant, vowel, inflection, volume, (uint) rand(), data.value, max_length);
    zox_instance(prefab_sound);
    zox_name("voice");
    zox_set_ptr(e, SoundData, data);
    zox_set(e, SoundLength, { voice_syllable_length(voice) });
    zox_set(e, TriggerSound, { zox_dirty_trigger });
    set_sound_source(world, e, source);
    return e;
}
