#define noise_wave_function(index, amplitude)\
    float noise_wave_##index(float time)\
    {\
        return amplitude * (time + (rand() / (float) RAND_MAX) * 2.0f - 1.0f);\
    }

// added time

noise_wave_function(0, 0.04f)
noise_wave_function(1, 0.08f)
noise_wave_function(2, 0.12f)
noise_wave_function(3, 0.16f)