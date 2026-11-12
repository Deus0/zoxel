// 3D sounds: a sound with a SoundPosition is heard from the listener
//  - quieter with distance, like unity's logarithmic rolloff: full volume within sound_min_distance,
//      then min / distance, fading to silent by sound_max_distance
//  - panned by its direction from the listener, equal power, never fully into one ear
//  - sounds without a position (ui, music) play in the middle at full volume
//  - the mixer (sounds/mix) applies them, the listener is the entity tagged SoundListener (the game camera)
#define sound_min_distance 3.0f
#define sound_max_distance 48.0f
#define sound_fade_distance 12.0f   // over the last of max distance, it fades to silent
#define sound_max_pan 0.75f         // hard left / right is unnatural on headphones
#define sound_silent_gain 0.005f
#define sound_sqrt2 1.41421356f

typedef struct {
    float left;
    float right;
} SoundGains;

static const SoundGains sound_gains_center = { 1, 1 };

// how loud a sound is at a distance (0 to 1)
static inline float sound_distance_gain(float distance) {
    if (distance <= sound_min_distance) {
        return 1;
    }
    if (distance >= sound_max_distance) {
        return 0;
    }
    float gain = sound_min_distance / distance;
    const float fade_start = sound_max_distance - sound_fade_distance;
    if (distance > fade_start) {
        gain *= 1 - (distance - fade_start) / sound_fade_distance;
    }
    return gain;
}

// left and right gains of a sound at source, heard by a listener (forward is -z, right is +x)
SoundGains sound_spatial_gains(float3 listener, float4 listener_rotation, float3 source) {
    const float3 delta = float3_subtract(source, listener);
    const float distance = float3_length(delta);
    const float gain = sound_distance_gain(distance);
    if (distance < 0.001f) {
        return (SoundGains) { gain, gain };
    }
    const float3 right = float4_rotate_float3(listener_rotation, float3_right);
    // close by (your own footsteps) it barely pans
    const float closeness = fminf(1, distance / sound_min_distance);
    const float pan = sound_max_pan * closeness * float3_dot(float3_scale(delta, 1 / distance), right);
    // equal power, scaled so the middle is full volume in both ears
    const float angle = (pan + 1) * (float) M_PI / 4;
    return (SoundGains) {
        gain * fminf(1, sound_sqrt2 * cosf(angle)),
        gain * fminf(1, sound_sqrt2 * sinf(angle)),
    };
}

static inline byte sound_gains_silent(SoundGains gains) {
    return gains.left < sound_silent_gain && gains.right < sound_silent_gain;
}

// a sound plays from where a source entity is (a character jumping, getting hit)
void set_sound_source(ecs* world, entity sound, entity source) {
    if (!sound || !zox_valid(source) || !zox_has(source, Position3D)) {
        return;
    }
    zox_set(sound, SoundPosition, { zox_getv(source, Position3D) });
}
