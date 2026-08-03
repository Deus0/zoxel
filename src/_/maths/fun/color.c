


static inline color seed_color(lint *seed, lint shift) {
    *seed += shift;
    byte r = seed_range(*seed, 0, 255);
    *seed += shift;
    byte g = seed_range(*seed, 0, 255);
    *seed += shift;
    byte b = seed_range(*seed, 0, 255);
    return (color) { r, g, b, 255 };
}
