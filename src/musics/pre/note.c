entity spawn_prefab_note(ecs *world) {
    zox_prefab();
    zox_prefab_name("note");
    zox_add(e, Note);
    zox_set(e, InstrumentType, { instrument_piano });
    zox_set(e, SoundFrequencyIndex, { 32 });
    zox_set(e, SoundVolume, { 1 });
    zox_set(e, SoundLength, { 0 });
    zox_set(e, NoteStart, { 0 });
    return e;
}

// Notes are ordered children of their music, in order of their start (seconds), each rings for its length
//  - they overlap freely: chords start together, a held note rings under the next ones
entity spawn_note(ecs *world, entity prefab, entity music, int note, byte instrument, double start, float length, float volume) {
    zox_instance(prefab);
    zox_name("note");
    zox_set_parent(world, e, music);
    zox_set(e, InstrumentType, { instrument });
    zox_set(e, SoundFrequencyIndex, { note });
    zox_set(e, NoteStart, { start });
    zox_set(e, SoundLength, { length });
    zox_set(e, SoundVolume, { volume });
    return e;
}
