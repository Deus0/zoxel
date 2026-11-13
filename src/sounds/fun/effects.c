// Sound effects: the table of what the world's actions sound like, tuned by tst sound_balance (types in dat/effects.c)

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

// materials, what a hit sounds like by what it hits (doc/sounds.md), kinds link one (link_hit_sound)
const SoundEffect hit_sound_flesh = { "flesh", instrument_impact, 34, 38, 0.1f, 0.13f, 1.9f, 2.1f };    // a body
const SoundEffect hit_sound_slime = { "slime", instrument_squelch, 40, 44, 0.18f, 0.22f, 1.7f, 1.9f };  // a wet squelch
const SoundEffect hit_sound_stone = { "stone", instrument_knock, 45, 49, 0.1f, 0.12f, 2.5f, 2.7f };     // a dry knock
const SoundEffect hit_sound_dirt = { "dirt", instrument_thud, 36, 40, 0.14f, 0.17f, 2.6f, 2.8f };       // a soft thud
const SoundEffect hit_sound_sand = { "sand", instrument_crunch, 44, 48, 0.14f, 0.17f, 3.0f, 3.2f };     // a grainy crunch
const SoundEffect hit_sound_wood = { "wood", instrument_tock, 43, 47, 0.14f, 0.16f, 2.8f, 3.0f };       // a hollow tock
const SoundEffect hit_sound_leaves = { "leaves", instrument_rustle, 48, 52, 0.2f, 0.24f, 1.4f, 1.6f };  // a rustle, plants too
// all of them, for tests
const SoundEffect* hit_sounds[] = { &hit_sound_flesh, &hit_sound_slime, &hit_sound_stone, &hit_sound_dirt,
    &hit_sound_sand, &hit_sound_wood, &hit_sound_leaves };
#define hit_sounds_count 7

// a sound picked within an effect's ranges
static inline entity spawn_sound_effect_data(ecs* world, const SoundEffect* effect) {
    const byte note = (byte) rand_range(effect->note_min, effect->note_max + 1);
    return spawn_sound_generated(world, prefab_sound_generated, effect->instrument, note_frequencies[note],
        frand_range(effect->seconds_min, effect->seconds_max),
        frand_range(effect->volume_min, effect->volume_max) * get_volume_sfx());
}

static inline entity spawn_sound_effect_sound(ecs* world, SoundEffectType type) {
    return type < sound_effect_end ? spawn_sound_effect_data(world, &sound_effects[type]) : 0;
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
