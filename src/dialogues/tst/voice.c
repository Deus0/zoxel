// tst: voice_sounds, npcs babble dialogue in their own voices (sounds/bop/voice.c, dialogues/fun/voice.c)
//  - syllables: "Hello there!" says he-llo the-re, a closing consonant is quiet, a word without vowels
//      says its first letter, a question rises
//  - voices from seeds: deep ones and high ones, the same seed the same voice
//  - a deep voice sounds lower than a tiny one, near its pitch (autocorrelation)
//  - vowels sound different: an i is brighter than a u, an s hisses brighter than an m hums
//  - writes tst/voices.wav, a line said by a few voices, to listen to
//  - run: bash bsh/tst.sh voice_sounds
#define zox_tst_voice_max 8192

static inline int zox_tst_count_syllables(const char* text) {
    int count = 0;
    DialogueSyllable syllable;
    for (int i = 0; text[i]; i++) {
        count += dialogue_syllable_at(text, i, &syllable);
    }
    return count;
}

// the pitch of samples, by autocorrelation (60 to 700hz)
static inline float zox_tst_pitch(const float* samples, uint length) {
    int best_lag = 0;
    double best = 0;
    for (int lag = sound_sample_rate / 700; lag <= sound_sample_rate / 60; lag++) {
        double sum = 0;
        for (uint j = 0; j + lag < length; j++) {
            sum += samples[j] * samples[j + lag];
        }
        if (sum > best) {
            best = sum;
            best_lag = lag;
        }
    }
    return best_lag ? sound_sample_rate / (float) best_lag : 0;
}

// how bright a sound is: its change from sample to sample, to its size
static inline float zox_tst_brightness(const float* samples, uint from, uint to) {
    double change = 0, power = 0;
    for (uint j = from + 1; j < to; j++) {
        const float d = samples[j] - samples[j - 1];
        change += d * d;
        power += samples[j] * samples[j];
    }
    return power > 0 ? (float) (change / power) : 0;
}

byte zox_tst_voice_sounds_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_voice_sounds_check(ecs* world) {
    (void) world;
    // syllables
    zox_tst_check(zox_tst_count_syllables("Hello there!") == 4, "\"Hello there!\" has [%i] syllables, not 4", zox_tst_count_syllables("Hello there!"));
    zox_tst_check(zox_tst_count_syllables("I am") == 2, "\"I am\" has [%i] syllables, not 2", zox_tst_count_syllables("I am"));
    zox_tst_check(zox_tst_count_syllables("cat") == 1, "\"cat\" has [%i] syllables, not 1", zox_tst_count_syllables("cat"));
    zox_tst_check(zox_tst_count_syllables("strong") == 1, "\"strong\" has [%i] syllables, not 1", zox_tst_count_syllables("strong"));
    zox_tst_check(zox_tst_count_syllables("... 42") == 0, "punctuation and numbers say something");
    DialogueSyllable syllable;
    zox_tst_check(dialogue_syllable_at("Hello", 0, &syllable) && syllable.consonant == 'h' && syllable.vowel == 'e',
        "\"Hello\" starts [%c%c], not [he]", syllable.consonant, syllable.vowel);
    zox_tst_check(dialogue_syllable_at("Hmm?", 0, &syllable) && syllable.consonant == 'h' && syllable.inflection > 0,
        "\"Hmm?\" doesn't rise on its h");
    zox_tst_check(dialogue_syllable_at("yes", 0, &syllable) && syllable.consonant == 'y' && syllable.vowel == 'e',
        "\"yes\" starts with y as a consonant");
    // voices from seeds
    int deep = 0, high = 0, ringing = 0;
    for (uint seed = 1; seed <= 64; seed++) {
        const Voice voice = voice_from_seed(seed);
        const Voice again = voice_from_seed(seed);
        zox_tst_check(!memcmp(&voice, &again, sizeof(Voice)), "seed [%u] made two voices", seed);
        deep += voice.pitch < 130;
        high += voice.pitch > 250;
        ringing += voice.ring > 0;
    }
    zox_tst_check(deep > 4 && high > 4, "64 seeds made [%i] deep and [%i] high voices", deep, high);
    zox_tst_check(ringing > 0, "64 seeds made no alien voices");
    // pitch
    static float samples[zox_tst_voice_max];
    Voice low = { .pitch = 100, .formant = 0.85f, .speed = 1.4f, .tilt = 1.2f };
    Voice tiny = { .pitch = 420, .formant = 1.3f, .speed = 1.4f, .tilt = 1.2f };
    uint length = generate_voice_syllable(&low, 0, 'a', 0, 1, 1, samples, zox_tst_voice_max);
    const float low_pitch = zox_tst_pitch(samples, length);
    length = generate_voice_syllable(&tiny, 0, 'a', 0, 1, 1, samples, zox_tst_voice_max);
    const float tiny_pitch = zox_tst_pitch(samples, length);
    zox_log("[tst] a deep voice at [%.0fhz] (of 100), a tiny one at [%.0fhz] (of 420)", low_pitch, tiny_pitch);
    zox_tst_check(low_pitch > 75 && low_pitch < 135, "a 100hz voice sounds at [%.0fhz]", low_pitch);
    zox_tst_check(tiny_pitch > 320 && tiny_pitch < 560, "a 420hz voice sounds at [%.0fhz]", tiny_pitch);
    // vowels and consonants
    Voice normal = { .pitch = 160, .formant = 1, .speed = 1.5f, .tilt = 1.2f };
    length = generate_voice_syllable(&normal, 0, 'i', 0, 1, 1, samples, zox_tst_voice_max);
    const float bright_i = zox_tst_brightness(samples, 0, length);
    length = generate_voice_syllable(&normal, 0, 'u', 0, 1, 1, samples, zox_tst_voice_max);
    const float bright_u = zox_tst_brightness(samples, 0, length);
    zox_tst_check(bright_i > bright_u * 1.5f, "an i [%.4f] isn't brighter than a u [%.4f]", bright_i, bright_u);
    const uint onset = (uint) (0.03f * sound_sample_rate);
    generate_voice_syllable(&normal, 's', 'a', 0, 1, 1, samples, zox_tst_voice_max);
    const float bright_s = zox_tst_brightness(samples, 0, onset);
    generate_voice_syllable(&normal, 'm', 'a', 0, 1, 1, samples, zox_tst_voice_max);
    const float bright_m = zox_tst_brightness(samples, 0, onset);
    zox_tst_check(bright_s > bright_m * 3, "an s [%.4f] doesn't hiss brighter than an m [%.4f]", bright_s, bright_m);
    // peaks and nans
    float peak = 0;
    byte finite = 1;
    for (uint j = 0; j < length; j++) {
        peak = fmaxf(peak, fabsf(samples[j]));
        finite &= isfinite(samples[j]);
    }
    zox_tst_check(finite && peak <= 0.91f && peak > 0.5f, "a syllable peaks at [%.3f]", peak);
    // listen: a line said by a few voices
    const char* line = "Hello there! Have you seen my lost cat? Hmm...";
    const uint gap = (uint) (0.14f * sound_sample_rate);
    const uint total = 6 * 3 * sound_sample_rate;
    float* mix = calloc(total, sizeof(float));
    uint at = 0;
    for (uint seed = 3; seed < 9; seed++) {
        const Voice voice = voice_from_seed(seed * 7);
        for (int i = 0; line[i] && at + zox_tst_voice_max < total; i++) {
            if (dialogue_syllable_at(line, i, &syllable)) {
                length = generate_voice_syllable(&voice, syllable.consonant, syllable.vowel, syllable.inflection, 0.6f * syllable.volume, i, samples, zox_tst_voice_max);
                for (uint j = 0; j < length; j++) {
                    mix[at + j] += samples[j];
                }
                at += gap;
            }
        }
        at += sound_sample_rate / 2;
    }
    zox_tst_write_wav(zox_tst_path("voices.wav"), mix, at);
    free(mix);
}

void zox_tsts_dialogues() {
    zox_tst_add("voice_sounds", zox_tst_voice_sounds_ready, zox_tst_voice_sounds_check, 30);
    zox_tst_set_setup("voice_sounds", NULL, 1);
}
