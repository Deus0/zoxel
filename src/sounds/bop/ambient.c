// Ambient instruments: soft and atmospheric (felt piano, harp, music box, cello, choir, deep bell)
//  - plucked and struck ones decay on their own, each harmonic faster than the one below
//  - bowed and sung ones sustain, with a slow attack and vibrato (see instrument_envelopes)
//  - they're normalized when generated, so they play at the same loudness

// deterministic noise from time, so a note sounds the same every time (rand isn't thread safe)
static inline float ambient_noise(float time) {
    const float x = sinf(time * 12989.8f) * 43758.547f;
    return (x - floorf(x)) * 2.0f - 1.0f;
}

// a felt piano: soft upper harmonics that die quickly, slightly stretched like real strings
float soft_piano_sound(float time, float frequency) {
    const float inharmonicity = 0.0004f;
    float value = 0;
    for (int n = 1; n <= 10; n++) {
        const float partial = frequency * n * sqrtf(1 + inharmonicity * n * n);
        if (partial > 9000) {
            break;
        }
        const float amplitude = 1.0f / powf((float) n, 1.6f);
        const float decay = expf(-time * (0.9f + 0.45f * n));
        value += amplitude * decay * sinf(2 * M_PI * partial * time);
    }
    return value;
}

// a harp: plucked near its end, bright at first then mellow
float harp_sound(float time, float frequency) {
    const float pluck_position = 0.2f;
    float value = 0;
    for (int n = 1; n <= 14; n++) {
        const float partial = frequency * n;
        if (partial > 10000) {
            break;
        }
        const float amplitude = fabsf(sinf(n * M_PI * pluck_position)) / (n * n);
        const float decay = expf(-time * (1.2f + 0.35f * powf((float) n, 1.3f)));
        value += amplitude * decay * sinf(2 * M_PI * partial * time);
    }
    return value;
}

// a music box: metal tines an octave up, their overtones aren't harmonic
float music_box_sound(float time, float frequency) {
    frequency *= 2;
    static const float ratios[] = { 1.0f, 3.9f, 9.6f };
    static const float amplitudes[] = { 1.0f, 0.35f, 0.12f };
    static const float decays[] = { 2.2f, 6.0f, 12.0f };
    float value = 0;
    for (int i = 0; i < 3; i++) {
        value += amplitudes[i] * expf(-time * decays[i]) * sinf(2 * M_PI * frequency * ratios[i] * time);
    }
    return value;
}

// a cello: bowed, a band limited saw with a warm body, vibrato fading in
float cello_sound(float time, float frequency) {
    const float vibrato_rate = 5.2f;
    const float vibrato_depth = 0.005f * fminf(1, time / 0.3f);
    // phase of a frequency wobbling by vibrato_depth
    const float vibrato_phase = -(vibrato_depth / vibrato_rate) * cosf(2 * M_PI * vibrato_rate * time);
    float value = 0;
    for (int n = 1; n <= 16; n++) {
        const float partial = frequency * n;
        if (partial > 6000) {
            break;
        }
        const float body = 1 + 0.6f * expf(-powf((partial - 500) / 250, 2));
        value += (body / n) * sinf(2 * M_PI * partial * (time + vibrato_phase));
    }
    return value + 0.02f * ambient_noise(time);
}

// a choir: three slightly detuned voices singing "ah" (their harmonics shaped by vowel formants)
float choir_sound(float time, float frequency) {
    static const float formants[] = { 700, 1220, 2600 };
    static const float widths[] = { 130, 150, 250 };
    static const float strengths[] = { 1.0f, 0.7f, 0.25f };
    static const float detunes[] = { 1.0f, 1.004f, 0.996f };
    const float vibrato = 0.004f * sinf(2 * M_PI * 5.0f * time);
    float value = 0;
    for (int v = 0; v < 3; v++) {
        const float voice = frequency * detunes[v] * (1 + vibrato);
        for (int n = 1; n <= 20; n++) {
            const float partial = voice * n;
            if (partial > 5000) {
                break;
            }
            float weight = 0.08f;
            for (int f = 0; f < 3; f++) {
                weight += strengths[f] * expf(-powf((partial - formants[f]) / widths[f], 2));
            }
            value += weight * sinf(2 * M_PI * partial * time + v);
        }
    }
    return value + 0.015f * ambient_noise(time);
}

// a deep bell: a hum an octave below, uneven overtones, a long ring
float bell_sound(float time, float frequency) {
    static const float ratios[] = { 0.5f, 1.0f, 1.183f, 1.506f, 2.0f, 2.514f, 2.662f, 3.011f, 4.166f };
    static const float amplitudes[] = { 0.6f, 1.0f, 0.7f, 0.5f, 0.45f, 0.3f, 0.25f, 0.2f, 0.1f };
    static const float decays[] = { 0.6f, 0.9f, 1.2f, 1.5f, 2.0f, 2.6f, 3.0f, 3.5f, 5.0f };
    float value = 0;
    for (int i = 0; i < 9; i++) {
        value += amplitudes[i] * expf(-time * decays[i]) * sinf(2 * M_PI * frequency * ratios[i] * time);
    }
    return value;
}
