// Impacts: hits that land on bodies, and swings through the air
//  - soft, not sharp: noise smoothed (sample and hold, blended) so it's muffled, not a hiss
//  - an impact: a body thump falling in pitch, a dull smack, and a short slap of muffled noise
//  - a whoosh: muffled noise that brightens then dulls, the swing's air
//  - small speakers lose the thump (under ~150hz), the smack and slap carry it there

// noise held and blended at a rate, the slower the rate the duller it sounds (keyed by its step, so
//  the rate must stay the same through a sound, a changing one keyed differently every sample: white noise)
static inline float impact_noise(float time, float rate, float key) {
    const float x = time * rate;
    const float i = floorf(x);
    const float t = x - i;
    const float a = ambient_noise(i * 0.0137f + key);
    const float b = ambient_noise((i + 1) * 0.0137f + key);
    return a + (b - a) * t * t * (3 - 2 * t);
}

// flesh on flesh: frequency is the body's thump (around 90 to 130hz)
float impact_sound(float time, float frequency) {
    // the thump starts higher and falls, like a hit body
    const float drop = 1.6f;
    const float drop_rate = 35;
    const float phase = 2 * M_PI * frequency * (time + drop * (1 - expf(-time * drop_rate)) / drop_rate);
    const float thump = 0.6f * sinf(phase) * expf(-time * 16);
    // a dull smack, not harmonic, what small speakers hear of it
    const float smack = 1.0f * sinf(2 * M_PI * frequency * 2.4f * time + 0.3f * sinf(phase)) * expf(-time * 18);
    // a slap of muffled noise, over in a few hundredths of a second
    const float slap = 1.0f * impact_noise(time, 2400, 0.3f) * expf(-time * 40);
    return thump + smack + slap;
}

// a swing through the air: wind rushing past, airy noise and a soft rising howl peaking mid swing, a dull body under it
float whoosh_sound(float time, float frequency) {
    const float swell = sinf(fminf(1, time * 4) * M_PI);
    const float dull = impact_noise(time, frequency * 8, 0.7f);
    const float rush = impact_noise(time, frequency * 20, 2.7f) * swell * 0.8f;
    const float body = dull * (1 - 0.4f * swell);
    const float howl = 0.2f * sinf(2 * M_PI * frequency * (1.5f + 0.5f * swell) * time) * swell;
    return rush + body + howl;
}

// Materials: what a hit sounds like by what it hits (hit_sounds in fun/effects.c, doc/sounds.md)
//  - first drafts by the local model, the pitch sweeps fixed (a sweep's phase is its pitch's integral)

// stone, a dry hard knock: a short click and three inharmonic rings that die fast
float knock_sound(float time, float frequency) {
    const float click = 1.2f * impact_noise(time, 4000, 0.1f) * expf(-time * 45);
    const float r1 = sinf(2 * M_PI * frequency * time) * expf(-time * 22);
    const float r2 = 0.8f * sinf(2 * M_PI * frequency * 2.3f * time) * expf(-time * 28);
    const float r3 = 0.5f * sinf(2 * M_PI * frequency * 3.9f * time) * expf(-time * 36);
    return click + r1 + r2 + r3;
}

// dirt, a soft dull thud: a low body falling in pitch and dull noise
float thud_sound(float time, float frequency) {
    const float drop = 1.2f, drop_rate = 25;
    const float phase = 2 * M_PI * frequency * (time + drop * (1 - expf(-time * drop_rate)) / drop_rate);
    const float body = sinf(phase) * expf(-time * 14);
    const float dull = 0.8f * impact_noise(time, frequency * 3, 0.5f) * expf(-time * 12);
    return body + dull;
}

// sand, a grainy crunch: muffled noise broken into grains by a slower noise
float crunch_sound(float time, float frequency) {
    const float gate = fminf(1, fmaxf(0, impact_noise(time, frequency * 0.6f, 0.2f) + 0.35f));
    const float noise = impact_noise(time, frequency * 8, 0.8f);
    return noise * gate * gate * 3 * expf(-time * 15) + 0.3f * sinf(2 * M_PI * frequency * time) * expf(-time * 25);
}

// wood, a hollow tock: a body ringing at its note and 2.76 times it, a tiny click at the start
float tock_sound(float time, float frequency) {
    const float click = 0.5f * impact_noise(time, 5000, 1.3f) * expf(-time * 60);
    const float r1 = 0.8f * sinf(2 * M_PI * frequency * time) * expf(-time * 18);
    const float r2 = 0.45f * sinf(2 * M_PI * frequency * 2.76f * time) * expf(-time * 24);
    return click + r1 + r2;
}

// slime, a wet squelch: a bubble's pitch rising fast, wobbling, over dull noise
float squelch_sound(float time, float frequency) {
    const float rise = 5;
    const float phase = 2 * M_PI * frequency * (time + rise * time * time) + 0.4f * sinf(2 * M_PI * 18 * time);
    const float bubble = sinf(phase) * expf(-time * 12);
    const float wet = 0.5f * impact_noise(time, frequency * 2, 2.1f) * expf(-time * 10);
    return bubble + wet;
}

// leaves (and plants), a soft rustle: noise that swells and fades
float rustle_sound(float time, float frequency) {
    const float swell = sinf(fminf(1, time / 0.22f) * M_PI);
    const float leaves = impact_noise(time, frequency * 14, 3.7f);
    const float twigs = 0.5f * impact_noise(time, frequency * 5, 1.1f);
    return (leaves + twigs) * swell;
}
