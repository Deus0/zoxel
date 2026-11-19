// Synth instruments: analog style voices for the menu theme (pad, glass, bass, lead, wind)
//  - band limited oscillators (polyblep saws and pulses, no aliasing whine on high notes)
//  - a resonant filter (state variable) shapes each, its cutoff moving through the note
//  - whole notes at once (they keep state: phases and filters), the instrument's attack, release and
//      normalizing come after (generate_instrument_samples)

// smooths a saw or pulse's jump, dt is the phase step a sample
static inline float synth_blep(float t, float dt) {
    if (t < dt) {
        t /= dt;
        return t + t - t * t - 1;
    }
    if (t > 1 - dt) {
        t = (t - 1) / dt;
        return t * t + t + t + 1;
    }
    return 0;
}

static inline float synth_saw(float phase, float dt) {
    return 2 * phase - 1 - synth_blep(phase, dt);
}

// a pulse, high for width of its cycle
static inline float synth_pulse(float phase, float dt, float width) {
    float shifted = phase + 1 - width;
    shifted -= floorf(shifted);
    return (phase < width ? 1.0f : -1.0f) + synth_blep(phase, dt) - synth_blep(shifted, dt);
}

static inline double synth_advance(double phase, double step) {
    phase += step;
    return phase - floor(phase);
}

// a state variable filter (trapezoidal), low and band pass outputs
typedef struct {
    float ic1;
    float ic2;
    float low;
    float band;
} SynthFilter;

static inline void synth_filter(SynthFilter* f, float x, float cutoff, float q) {
    cutoff = clampf(cutoff, 20, sample_rate_f * 0.45f);
    const float g = tanf(M_PI * cutoff / sample_rate_f);
    const float k = 1 / q;
    const float a1 = 1 / (1 + g * (g + k));
    const float a2 = g * a1;
    const float a3 = g * a2;
    const float v3 = x - f->ic2;
    const float v1 = a1 * f->ic1 + a2 * v3;
    const float v2 = f->ic2 + a2 * f->ic1 + a3 * v3;
    f->ic1 = 2 * v1 - f->ic1;
    f->ic2 = 2 * v2 - f->ic2;
    f->low = v2;
    f->band = v1;
}

// a pad: three detuned saws, its filter slowly opening as it swells, breathing with a slow wobble
void synth_pad_note(float frequency, float* samples, uint length) {
    static const float detunes[] = { 1.0f, 1.0059f, 0.9943f };
    double phases[3] = { 0, 0.33, 0.67 };
    SynthFilter filter = { 0 };
    for (uint j = 0; j < length; j++) {
        const float time = j / sample_rate_f;
        float value = 0;
        for (int v = 0; v < 3; v++) {
            const float dt = frequency * detunes[v] / sample_rate_f;
            value += synth_saw((float) phases[v], dt);
            phases[v] = synth_advance(phases[v], dt);
        }
        const float swell = 1 - expf(-time / 0.9f);
        const float wobble = 0.15f * sinf(2 * M_PI * 0.35f * time);
        synth_filter(&filter, value, frequency * (1.2f + 3.5f * swell + wobble), 1.1f);
        samples[j] = filter.low + 0.01f * ambient_noise(time);
    }
}

// glass: a struck fm tone, bright as it's hit then a pure ring, a faint shimmer above (for arpeggios)
void synth_glass_note(float frequency, float* samples, uint length) {
    double carrier = 0, modulator = 0, shimmer = 0;
    const double step = frequency / sample_rate_f;
    for (uint j = 0; j < length; j++) {
        const float time = j / sample_rate_f;
        const float index = 2.2f * expf(-time * 7) + 0.25f;
        const float mod = index * sinf(2 * M_PI * modulator) + 0.08f * sinf(2 * M_PI * shimmer);
        samples[j] = expf(-time * 2.4f) * sinf(2 * M_PI * carrier + mod);
        carrier = synth_advance(carrier, step);
        modulator = synth_advance(modulator, step * 2);
        shimmer = synth_advance(shimmer, step * 3.51);
    }
}

// a bass: a saw and a sine under it, a resonant filter plucked open then closing to a dark hum
//  - its saw's harmonics carry it on small speakers, which lose its root
void synth_bass_note(float frequency, float* samples, uint length) {
    double phase = 0;
    const float dt = frequency / sample_rate_f;
    SynthFilter filter = { 0 };
    for (uint j = 0; j < length; j++) {
        const float time = j / sample_rate_f;
        const float saw = synth_saw((float) phase, dt);
        const float sine = sinf(2 * M_PI * phase);
        synth_filter(&filter, saw, frequency * (2.5f + 9 * expf(-time * 7)), 1.6f);
        samples[j] = (filter.low + 0.6f * sine) * (0.75f + 0.25f * expf(-time * 3));
        phase = synth_advance(phase, dt);
    }
}

// a lead: a hollow pulse, its width slowly moving, scooping up into its note, a vibrato fading in
void synth_lead_note(float frequency, float* samples, uint length) {
    double phase = 0, octave = 0;
    SynthFilter filter = { 0 };
    for (uint j = 0; j < length; j++) {
        const float time = j / sample_rate_f;
        const float scoop = 1 - 0.015f * expf(-time / 0.04f);
        const float vibrato = 1 + 0.006f * fminf(1, fmaxf(0, time - 0.25f) / 0.4f) * sinf(2 * M_PI * 5.3f * time);
        const float dt = frequency * scoop * vibrato / sample_rate_f;
        const float width = 0.5f + 0.18f * sinf(2 * M_PI * 0.6f * time);
        // a pulse's average is its width's offset from half, taken off (it shifted the whole mix)
        const float pulse = synth_pulse((float) phase, dt, width) - (2 * width - 1);
        const float value = pulse + 0.25f * sinf(2 * M_PI * octave);
        synth_filter(&filter, value, frequency * (3 + 3 * expf(-time * 4)), 0.9f);
        samples[j] = filter.low;
        phase = synth_advance(phase, dt);
        octave = synth_advance(octave, dt * 2);
    }
}

// wind: noise whistling through a narrow band at its note, the band drifting
void synth_wind_note(float frequency, float* samples, uint length) {
    SynthFilter filter = { 0 };
    for (uint j = 0; j < length; j++) {
        const float time = j / sample_rate_f;
        const float drift = 1 + 0.12f * sinf(2 * M_PI * 0.23f * time) + 0.05f * sinf(2 * M_PI * 0.61f * time);
        synth_filter(&filter, ambient_noise(time), frequency * drift, 7);
        samples[j] = filter.band;
    }
}
