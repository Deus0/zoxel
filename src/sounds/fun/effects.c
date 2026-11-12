// Sound effects: what the world's actions sound like, all in one place (tuned by tst sound_balance)
//  - an effect is an instrument, a range of notes (note_frequencies), lengths and volumes (of the sfx volume)
//  - each spawn picks within its ranges, plays from its source (3D) or in the middle (no source)
//  - dialogue is spoken in voices (sounds/bop/voice.c), not effects
//  - hits are impacts (flesh on flesh), swings whooshes, soft not sharp (sounds/bop/impacts.c)
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
    sound_effect_block_damage,  // a block hit by a punch or tool (block_damage.c)
    sound_effect_block_break,
    sound_effect_level_up,
    sound_effect_click,         // ui, in the middle
    sound_effect_click_rare,
    sound_effect_start,         // the start screen, on to the main menu
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
    [sound_effect_hit] = { "hit", instrument_impact, 34, 38, 0.1f, 0.13f, 1.9f, 2.1f },
    [sound_effect_swing] = { "swing", instrument_whoosh, 40, 44, 0.2f, 0.24f, 1.6f, 1.8f },
    [sound_effect_out_of_range] = { "out of range", instrument_flute, 50, 50, 0.3f, 0.3f, 1.2f, 1.2f },
    [sound_effect_no_resource] = { "no resource", instrument_piano, 36, 36, 0.6f, 0.6f, 1.4f, 1.4f },
    [sound_effect_block_hit] = { "block hit", instrument_impact, 40, 44, 0.12f, 0.14f, 1.8f, 1.9f },
    [sound_effect_block_hard] = { "block hard", instrument_piano_square, 45, 47, 0.26f, 0.26f, 0.75f, 0.75f },
    [sound_effect_consume] = { "consume", instrument_guitar, 40, 45, 0.4f, 0.4f, 1.5f, 1.5f },
    [sound_effect_place] = { "place", instrument_guitar, 38, 43, 0.4f, 0.4f, 1.5f, 1.5f },
    [sound_effect_aura] = { "aura", instrument_impact, 33, 35, 0.16f, 0.16f, 1.8f, 1.8f },
    [sound_effect_block_damage] = { "block damage", instrument_impact, 40, 44, 0.12f, 0.14f, 1.8f, 1.9f },
    [sound_effect_block_break] = { "block break", instrument_piano, 40, 48, 0.7f, 0.7f, 1.4f, 1.4f },
    [sound_effect_level_up] = { "level up", instrument_piano, 44, 47, 2.8f, 2.8f, 1.4f, 1.4f },
    [sound_effect_click] = { "click", instrument_flute, 27, 33, 0.3f, 0.45f, 0.9f, 1.2f },
    [sound_effect_click_rare] = { "click rare", instrument_harp, 52, 52, 0.4f, 0.5f, 1.2f, 1.4f },
    [sound_effect_start] = { "start", instrument_piano, 40, 40, 2.8f, 2.8f, 1.1f, 1.3f },
};

static inline entity spawn_sound_effect_sound(ecs* world, SoundEffectType type) {
    if (type >= sound_effect_end) {
        return 0;
    }
    const SoundEffect effect = sound_effects[type];
    const byte note = (byte) rand_range(effect.note_min, effect.note_max + 1);
    return spawn_sound_generated(world, prefab_sound_generated, effect.instrument, note_frequencies[note],
        frand_range(effect.seconds_min, effect.seconds_max),
        frand_range(effect.volume_min, effect.volume_max) * get_volume_sfx());
}

// plays an effect from a source entity (3D), or in the middle when source is 0
entity spawn_sound_effect(ecs* world, SoundEffectType type, entity source) {
    const entity e = spawn_sound_effect_sound(world, type);
    set_sound_source(world, e, source);
    return e;
}

// plays an effect from a place (3D), i.e. a block
entity spawn_sound_effect_at(ecs* world, SoundEffectType type, float3 position) {
    const entity e = spawn_sound_effect_sound(world, type);
    if (e) {
        zox_set(e, SoundPosition, { position });
    }
    return e;
}
