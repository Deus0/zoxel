// Sound effects: what the world's actions sound like, all in one place (tuned by tst sound_balance)
//  - an effect is an instrument, a range of notes (note_frequencies), lengths and volumes (of the sfx volume)
//  - each spawn picks within its ranges, plays from its source (3D) or in the middle (no source)
//  - small speakers barely play under ~150hz, so effects keep their body above that, or nobody hears them
typedef enum {
    sound_effect_jump,
    sound_effect_land,
    sound_effect_hit,           // a character takes damage
    sound_effect_swing,         // a punch at nothing
    sound_effect_out_of_range,
    sound_effect_no_resource,
    sound_effect_block_hit,
    sound_effect_block_hard,    // a block that can't break
    sound_effect_consume,
    sound_effect_place,
    sound_effect_aura,
    sound_effect_end
} SoundEffectType;

typedef struct {
    const char* name;
    byte instrument;
    byte note_min;
    byte note_max;
    float seconds_min;
    float seconds_max;
    float volume_min;
    float volume_max;
} SoundEffect;

const SoundEffect sound_effects[sound_effect_end] = {
    [sound_effect_jump] = { "jump", instrument_edm, 38, 43, 0.18f, 0.24f, 1.6f, 2.0f },
    [sound_effect_land] = { "land", instrument_edm, 31, 35, 0.3f, 0.36f, 2.2f, 2.6f },
    [sound_effect_hit] = { "hit", instrument_piano_square, 40, 42, 0.4f, 0.5f, 0.85f, 1.0f },
    [sound_effect_swing] = { "swing", instrument_flute, 45, 47, 0.22f, 0.28f, 1.2f, 1.4f },
    [sound_effect_out_of_range] = { "out of range", instrument_flute, 50, 50, 0.3f, 0.3f, 1.2f, 1.2f },
    [sound_effect_no_resource] = { "no resource", instrument_piano, 36, 36, 0.6f, 0.6f, 1.4f, 1.4f },
    [sound_effect_block_hit] = { "block hit", instrument_piano, 36, 41, 0.4f, 0.4f, 1.4f, 1.6f },
    [sound_effect_block_hard] = { "block hard", instrument_piano_square, 45, 47, 0.26f, 0.26f, 0.75f, 0.75f },
    [sound_effect_consume] = { "consume", instrument_guitar, 40, 45, 0.4f, 0.4f, 1.5f, 1.5f },
    [sound_effect_place] = { "place", instrument_guitar, 38, 43, 0.4f, 0.4f, 1.5f, 1.5f },
    [sound_effect_aura] = { "aura", instrument_piano_square, 40, 40, 0.5f, 0.5f, 0.75f, 0.75f },
};

// plays an effect from a source entity (3D), or in the middle when source is 0
entity spawn_sound_effect(ecs* world, SoundEffectType type, entity source) {
    if (type >= sound_effect_end) {
        return 0;
    }
    const SoundEffect effect = sound_effects[type];
    const byte note = (byte) rand_range(effect.note_min, effect.note_max + 1);
    const entity e = spawn_sound_generated(world, prefab_sound_generated, effect.instrument, note_frequencies[note],
        frand_range(effect.seconds_min, effect.seconds_max),
        frand_range(effect.volume_min, effect.volume_max) * get_volume_sfx());
    set_sound_source(world, e, source);
    return e;
}
