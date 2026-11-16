// animated water: a texture's frames, ripples moving over a fill colour
//  - three waves with whole wave numbers across the texture and whole cycles over the frames: it tiles with itself
//    and its last frame leads back into its first
//  - quantized to a few shades (pixel art), the crests brightest
#define zox_water_frames 16         // a loop
#define zox_water_frame_rate 8.0f   // frames a second, a 2 second loop
#define zox_water_alpha 170

static inline float water_ripple(int x, int y, int frame, int2 size, byte frames, int seed) {
    const float u = x / (float) size.x, v = y / (float) size.y, t = frame / (float) frames;
    const float p = (seed % 64) / 64.0f;
    const float tau = 2.0f * (float) M_PI;
    return (sinf(tau * (u + v + t + p)) + sinf(tau * (v - 2 * u + t + 2 * p)) + sinf(tau * (u - 2 * v - 2 * t + 3 * p))) / 3.0f;
}

// frames back to back in data (frames * size.x * size.y colors)
void generate_texture_water(color* data, int2 size, byte frames, color fill, int seed) {
    const int shades[4] = { -22, -8, 8, 40 };
    for (int f = 0; f < frames; f++) {
        for (int y = 0; y < size.y; y++) {
            for (int x = 0; x < size.x; x++) {
                const float ripple = water_ripple(x, y, f, size, frames, seed);
                const int shade = shades[ripple > 0.55f ? 3 : (ripple > 0 ? 2 : (ripple > -0.5f ? 1 : 0))];
                data[(f * size.y + y) * size.x + x] = (color) {
                    int_min(int_max(fill.r + shade, 0), 255),
                    int_min(int_max(fill.g + shade, 0), 255),
                    int_min(int_max(fill.b + shade, 0), 255),
                    fill.a };
            }
        }
    }
}
