float color_rgb_to_hue(color_rgb value) {
    float red = (int) value.r / 255.0f;
    float green = (int) value.g / 255.0f;
    float blue = (int) value.b / 255.0f;
    float max = float_max(red, float_max(green, blue));
    float min = float_min(red, float_min(green, blue));
    float delta = (max - min);
    int multi = 60; // 42; // 60
    float hue = 0; // (int) ceil
    // a grey has no hue: dividing by its zero delta gave NaN, and NaN to a byte is undefined (hsv_to_color_rgb)
    if (delta == 0) {
        return 0;
    }
    if (red == max) {
        hue = (multi * ((green - blue) / delta)); //  % 6;
    } else if (green == max) {
        hue = (multi * ((int) 2 + ((blue - red) / delta)));
    } else {
        hue = (multi * ((int) 4 + ((red - green) / delta)));
    }
    if (hue < 0) {
        hue = 360 + hue;
    }
    return hue;
}

static inline float color_rgb_to_saturation(color_rgb value) {
    float red = (int) value.r / 255.0f;
    float green = (int) value.g / 255.0f;
    float blue = (int) value.b / 255.0f;
    float max = float_max(red, float_max(green, blue));
    float min = float_min(red, float_min(green, blue));
    float delta = (max - min);
    if (max == 0) {
        return 0;
    } else {
        return 100.0f * (delta / max);
    }
}

static inline float color_rgb_to_value(color_rgb value) {
    float red = (int) value.r / 255.0f;
    float green = (int) value.g / 255.0f;
    float blue = (int) value.b / 255.0f;
    float max = float_max(red, float_max(green, blue));
    return max * 100;
}

static inline float3 color_rgb_to_hsv(color_rgb value) {
    return (float3) {
        color_rgb_to_hue(value),
        color_rgb_to_saturation(value),
        color_rgb_to_value(value)
    };
}

color_rgb hsv_to_color_rgb(float3 hsv) {
    float hue = hsv.x;
    float saturation = hsv.y / 100.0f;
    const float value = hsv.z / 100.0f;
    float chroma = value * saturation;
    float hue_ = fmod(hue / 60.0f, 6.0f); // zox_fmod
    float x = chroma * (1 - fabs(fmod(hue_, 2.0f) - 1));
    float r, g, b;
    if (hue_ >= 0 && hue_ < 1) {
        r = chroma;
        g = x;
        b = 0;
    } else if (hue_ >= 1 && hue_ < 2) {
        r = x;
        g = chroma;
        b = 0;
    } else if (hue_ >= 2 && hue_ < 3) {
        r = 0;
        g = chroma;
        b = x;
    } else if (hue_ >= 3 && hue_ < 4) {
        r = 0;
        g = x;
        b = chroma;
    } else if (hue_ >= 4 && hue_ < 5) {
        r = x;
        g = 0;
        b = chroma;
    } else {
        r = chroma;
        g = 0;
        b = x;
    }
    float m = value - chroma;
    r += m;
    g += m;
    b += m;
    // clamped to a byte: a float out of a byte's range made one is undefined, a saturation over 100 gave a channel
    //  below 0; gcc wrapped it, clang (web, -O3) folded the realm's stat colours, saw it, and dropped the rest of
    //  stats_realm_spawn_system as unreachable: the realm had no stats
    r = clampf(ceil(r * 255), 0, 255);
    g = clampf(ceil(g * 255), 0, 255);
    b = clampf(ceil(b * 255), 0, 255);
    return (color_rgb) { (byte) r, (byte) g, (byte) b };
}

color_rgb color_rgb_saturate(color_rgb in, float sat) {
    float3 hsv = color_rgb_to_hsv(in);
    hsv.y = float_min(hsv.y * sat, 100);    // a percent
    return hsv_to_color_rgb(hsv);
}

color_rgb color_rgb_darken(color_rgb in, float mul) {
    float3 hsv = color_rgb_to_hsv(in);
    hsv.z *= mul;
    return hsv_to_color_rgb(hsv);
}

static inline float2 float2_to_limits(float2 v) {
    if (v.x > v.y) {
        return (float2) { v.y, v.x };
    } else if (v.x == v.y) {
        return (float2) { v.x, v.y + 1 };
    }else {
        return v;
    }
}

static inline color_rgb seed_to_color_rgb_range(lint seed, float2 hue, float2 value, float2 saturation) {
    float3 hsv = seed_to_hsv(seed, hue, value, saturation);
    return hsv_to_color_rgb(hsv);
    // return hsv_to_color_rgb(seed, hue, value, saturation);
}

static inline color_rgb seed_to_color_rgb(lint seed) {
    float2 hue = (float2) { seed_range(seed, 0, 360), seed_range(seed, 0, 360) };
    float2 value = (float2) { seed_range(seed, 0, 100), seed_range(seed, 0, 100) };
    float2 saturation = (float2) { seed_range(seed, 0, 100), seed_range(seed, 0, 100) };
    hue = float2_to_limits(hue);
    value = float2_to_limits(value);
    saturation = float2_to_limits(saturation);
    return seed_to_color_rgb_range(seed, hue, value, saturation);
}
