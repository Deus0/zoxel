// Hit sounds: sound entities of the realm, what a block or character kind sounds like hit (doc/sounds.md)
//  - link_hit_sound: a kind (a realm block, a character prefab) spawns its sound entity, parented to the
//    realm or biome that spawned it, from a material (hit_sound_stone etc) nudged by that parent's seed,
//    and links it (HitSoundLink), character instances link their prefab's
//  - spawn_hit_sound / spawn_hit_sound_at: a hit plays what is hit's sound, a fallback effect without one
//  - spawn_character_hit_sound: a body's sound, and a voiced one (HitSoundVoiced) grunts in its own voice

static inline uint hit_sound_hash(uint x) {
    x ^= x >> 16;
    x *= 0x7feb352du;
    x ^= x >> 15;
    x *= 0x846ca68bu;
    return x ^ (x >> 16);
}

// a material nudged by a seed: a few semitones up or down, a little longer or shorter
SoundEffect nudge_hit_sound(const SoundEffect* material, lint seed) {
    SoundEffect effect = *material;
    const uint hash = hit_sound_hash((uint) seed * 31 + material->instrument);
    const int shift = (int) (hash % 5) - 2;
    const float stretch = 0.9f + 0.2f * ((hash >> 8) % 100) / 100.0f;
    effect.note_min = (byte) (effect.note_min + shift);
    effect.note_max = (byte) (effect.note_max + shift);
    effect.seconds_min *= stretch;
    effect.seconds_max *= stretch;
    return effect;
}

// spawns e's hit sound under parent (its realm or biome), returns it
entity link_hit_sound(ecs* world, entity e, entity parent, const SoundEffect* material) {
    const lint seed = zox_valid(parent) && zox_has(parent, Seed) ? zox_getv(parent, Seed) : 0;
    const SoundEffect effect = nudge_hit_sound(material, seed);
    const entity sound = zox_ins(world, prefab_realm_sound);
    zox_set_unique_name(sound, effect.name);
    zox_setv(sound, SoundEffectData, effect);
    if (material == &hit_sound_flesh) {
        zox_add(sound, HitSoundVoiced);
    }
    zox_set_parent(world, sound, parent);
    zox_set_link(world, e, HitSoundLink, sound);
    return sound;
}

// the linked sound, or the fallback effect without one
static inline entity spawn_hit_sound_linked(ecs* world, entity hit, SoundEffectType fallback) {
    const entity sound = zox_valid(hit) ? zox_get_link(world, hit, HitSoundLink) : 0;
    if (zox_valid(sound)) {
        const SoundEffectData* data = zox_get(sound, SoundEffectData);
        return spawn_sound_effect_data(world, &data->value);
    }
    return spawn_sound_effect_sound(world, fallback);
}

// plays from the hit entity (3D)
entity spawn_hit_sound(ecs* world, entity hit, SoundEffectType fallback, entity source) {
    const entity e = spawn_hit_sound_linked(world, hit, fallback);
    set_sound_source(world, e, source);
    return e;
}

// plays from a place, i.e. a block (hit is its block kind)
entity spawn_hit_sound_at(ecs* world, entity hit, SoundEffectType fallback, float3 position) {
    const entity e = spawn_hit_sound_linked(world, hit, fallback);
    if (e) {
        zox_set(e, SoundPosition, { position });
    }
    return e;
}

// a character hit: its body's sound, and a voiced body (no sound linked, a player, is too) grunts most hits
void spawn_character_hit_sound(ecs* world, entity character) {
    spawn_hit_sound(world, character, sound_effect_hit, character);
    const entity sound = zox_get_link(world, character, HitSoundLink);
    const byte voiced = !zox_valid(sound) || zox_has(sound, HitSoundVoiced);
    if (voiced && zox_has(character, Seed) && rand() % 4) {
        const Voice voice = voice_from_seed((uint) zox_getv(character, Seed));
        spawn_voice_grunt(world, &voice, 0.9f * get_volume_sfx(), character);
    }
}
