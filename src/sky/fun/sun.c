// a sun's light by its time of day (0 midnight, 0.5 noon): where it is, how bright, its colour warm near the horizon
const float sun_day_length = 1200;     // seconds, 20 minutes
const float sun_day_start = 0.3f;      // a realm starts in the morning
const float sun_tilt = 0.45f;          // radians its path leans from overhead, so noon's shadows have a side
const color_rgb sun_color = { 255, 246, 230 };
const float3 sky_night_tint = { 0.12f, 0.14f, 0.24f };     // a sunless sky's light: dark, blue, still playable

static inline float smootherf(float lo, float hi, float x) {
    const float t = clampf((x - lo) / (hi - lo), 0, 1);
    return t * t * (3 - 2 * t);
}

// its time of day at a realm's time
static inline float sun_time_of_day(double time, float day_length, float day_start) {
    const double days = time / (day_length > 0 ? day_length : sun_day_length) + day_start;
    return (float) (days - floor(days));
}

// towards it: rising in +x at 0.25, highest at 0.5, setting in -x at 0.75, under the ground at night
static inline float3 sun_direction_at(float time_of_day) {
    const float angle = (time_of_day - 0.25f) * 2 * (float) M_PI;
    const float height = sinf(angle);
    return float3_normalize((float3) { cosf(angle), height * cosf(sun_tilt), height * sinf(sun_tilt) });
}

// its colour: the sunset's low down, its own high up
static inline float3 sun_color_at(float3 direction, color_rgb color, color_rgb sunset) {
    return float3_lerp(color_rgb_to_float3(sunset), color_rgb_to_float3(color), smootherf(0, 0.35f, direction.y));
}

// its light on the world: its colour, none under the horizon
static inline float3 sun_light_at(float3 direction, float3 color) {
    return float3_scale(color, smootherf(-0.1f, 0.25f, direction.y));
}
