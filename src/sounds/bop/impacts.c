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

// a swing through the air: muffled air that brightens in the middle of the swing, frequency sets its body
float whoosh_sound(float time, float frequency) {
    const float swell = sinf(fminf(1, time * 4) * M_PI);
    const float dull = impact_noise(time, frequency * 8, 0.7f);
    const float bright = impact_noise(time, frequency * 20, 1.9f);
    return dull * (1 - 0.5f * swell) + bright * swell * 0.7f + 0.2f * sinf(2 * M_PI * frequency * 2 * time);
}
