// a realm's sound: what a kind sounds like hit (doc/sounds.md), not played itself, hits read it
entity spawn_prefab_realm_sound(ecs *world) {
    zox_prefab();
    zox_prefab_name("realm_sound");
    zox_set(e, SoundEffectData, { 0 });
    return e;
}
