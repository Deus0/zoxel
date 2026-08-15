entity spawn_prefab_note(ecs *world) {
    zox_prefab();
    zox_prefab_name("note");
    zox_add(e, Note);
    zox_prefab_set(e, InstrumentType, { instrument_piano });
    zox_prefab_set(e, SoundFrequencyIndex, { 32 });
    zox_prefab_set(e, SoundVolume, { 1 });
    zox_prefab_set(e, SoundLength, { 0 });
    return e;
}

entity spawn_note(ecs *world, entity prefab, int note, byte instrument, float length, float volume) {
    zox_instance(prefab);
    zox_name("note");
    zox_set(e, InstrumentType, { instrument });
    zox_set(e, SoundFrequencyIndex, { note });
    zox_set(e, SoundLength, { length });
    zox_set(e, SoundVolume, { volume });
    return e;
}