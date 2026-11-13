// tst: hit_sounds, every material sounds like itself (sounds/bop/hits.c, doc/sounds.md)
//  - each generates, isn't silent or broken, and dies away (its last fifth quieter than its first)
//  - each pair differs: brightness (change to size) by a fifth, or pitch by 3 semitones
//  - stone and sand brighter than dirt, slime and flesh duller than stone
//  - nudged by a seed (2 semitones at most), the same seed the same, another seed differs
//  - a grunt: in range (the distortion keeps it under its volume), loud enough, its own length
//  - writes bin/tst/instruments/grunt_1 to 6.wav, to listen to (the materials are written by sound_instruments)
//  - run: bash bsh/tst.sh hit_sounds
byte zox_tst_hit_sounds_ready(ecs* world) {
    (void) world;
    return 1;
}

static inline float zox_tst_hit_brightness(const float* samples, uint length) {
    double change = 0, power = 0;
    for (uint j = 1; j < length; j++) {
        change += (samples[j] - samples[j - 1]) * (samples[j] - samples[j - 1]);
        power += samples[j] * samples[j];
    }
    return power > 0 ? (float) (change / power) : 0;
}

static inline double zox_tst_hit_rms(const float* samples, uint length) {
    double power = 0;
    for (uint j = 0; j < length; j++) {
        power += samples[j] * samples[j];
    }
    return length ? sqrt(power / length) : 0;
}

void zox_tst_hit_sounds_check(ecs* world) {
    (void) world;
    float* samples = malloc(sound_sample_rate * sizeof(float));
    float brightness[hit_sounds_count];
    int notes[hit_sounds_count];
    for (byte k = 0; k < hit_sounds_count; k++) {
        const SoundEffect effect = *hit_sounds[k];
        notes[k] = (effect.note_min + effect.note_max) / 2;
        const uint length = (uint) (effect.seconds_max * sound_sample_rate);
        const byte made = generate_instrument_samples(effect.instrument, note_frequencies[notes[k]], effect.seconds_max, 1, 0, samples, length);
        zox_tst_check(made, "[%s] didn't generate", effect.name);
        int broken = 0;
        float peak = 0;
        for (uint j = 0; j < length; j++) {
            broken += !isfinite(samples[j]);
            peak = fmaxf(peak, fabsf(samples[j]));
        }
        const uint fifth = length / 5;
        const double head = zox_tst_hit_rms(samples, fifth), tail = zox_tst_hit_rms(samples + length - fifth, fifth);
        brightness[k] = zox_tst_hit_brightness(samples, length);
        zox_log("[tst] [%s] peak [%.2f] brightness [%.4f] head [%.3f] tail [%.3f]", effect.name, peak, brightness[k], head, tail);
        zox_tst_check(!broken, "[%s] has [%i] broken samples", effect.name, broken);
        zox_tst_check(peak > 0.3f && peak <= 1, "[%s] peak [%.2f]", effect.name, peak);
        zox_tst_check(tail < head * 0.5, "[%s] doesn't die away, head [%.3f] tail [%.3f]", effect.name, head, tail);
    }
    for (byte a = 0; a < hit_sounds_count; a++) {
        for (byte b = a + 1; b < hit_sounds_count; b++) {
            const float ratio = brightness[a] > brightness[b] ? brightness[a] / fmaxf(brightness[b], 1e-6f) : brightness[b] / fmaxf(brightness[a], 1e-6f);
            zox_tst_check(ratio > 1.2f || abs(notes[a] - notes[b]) >= 3, "[%s] and [%s] sound alike, brightness [%.4f] [%.4f], notes [%i] [%i]",
                hit_sounds[a]->name, hit_sounds[b]->name, brightness[a], brightness[b], notes[a], notes[b]);
        }
    }
    // hit_sounds: flesh 0, slime 1, stone 2, dirt 3, sand 4
    zox_tst_check(brightness[2] > brightness[3], "stone is duller than dirt");
    zox_tst_check(brightness[4] > brightness[3], "sand is duller than dirt");
    zox_tst_check(brightness[1] < brightness[2], "slime is brighter than stone");
    zox_tst_check(brightness[0] < brightness[2], "flesh is brighter than stone");
    // realms nudge them
    int nudged = 0, unsteady = 0, too_far = 0;
    for (byte k = 0; k < hit_sounds_count; k++) {
        const SoundEffect a = nudge_hit_sound(hit_sounds[k], 666), b = nudge_hit_sound(hit_sounds[k], 666);
        const SoundEffect c = nudge_hit_sound(hit_sounds[k], 777);
        unsteady += a.note_min != b.note_min || a.seconds_max != b.seconds_max;
        nudged += a.note_min != c.note_min || a.seconds_max != c.seconds_max;
        too_far += abs(a.note_min - hit_sounds[k]->note_min) > 2 || a.note_max - a.note_min != hit_sounds[k]->note_max - hit_sounds[k]->note_min;
    }
    zox_tst_check(!unsteady, "[%i] realm hit sounds changed with the same seed", unsteady);
    zox_tst_check(nudged, "two realm seeds gave the same hit sounds");
    zox_tst_check(!too_far, "[%i] realm hit sounds nudged over 2 semitones", too_far);
    // grunts
    for (uint seed = 1; seed <= 6; seed++) {
        const Voice voice = voice_from_seed(seed * 101);
        const float volume = 0.8f;
        const uint length = generate_voice_grunt(&voice, volume, seed, samples, sound_sample_rate);
        float peak = 0;
        int broken = 0;
        for (uint j = 0; j < length; j++) {
            broken += !isfinite(samples[j]);
            peak = fmaxf(peak, fabsf(samples[j]));
        }
        char path[64];
        snprintf(path, sizeof(path), "../tst/instruments/grunt_%u.wav", seed);
        zox_tst_write_wav(path, samples, length);
        const float seconds = length / (float) sound_sample_rate;
        zox_log("[tst] grunt [%u] pitch [%.0f] [%.2fs] peak [%.2f] rms [%.3f]", seed, voice.pitch, seconds, peak, zox_tst_hit_rms(samples, length));
        zox_tst_check(length && !broken, "grunt [%u] broken [%i] of [%u]", seed, broken, length);
        zox_tst_check(peak <= volume + 0.001f && peak > volume * 0.7f, "grunt [%u] peak [%.2f] for volume [%.2f]", seed, peak, volume);
        zox_tst_check(seconds > voice_syllable_length(&voice), "grunt [%u] [%.2fs] no longer than a syllable", seed, seconds);
    }
    free(samples);
}

void zox_tsts_hits() {
    zox_tst_add("hit_sounds", zox_tst_hit_sounds_ready, zox_tst_hit_sounds_check, 30);
}
