// Generates a music's notes from its seed (compose_song, like a dungeon from its seed)
//  - each step is a note (the melody, or a rest), its other voices are its child notes
//  - the music plays the composed song's tempo, its lead instrument is the music's InstrumentType
zox_sys2(MusicGenerateSystem) {
    if (nomusic) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Seed);
    zox_sys_in(InstrumentType);
    zox_sys_out(GenerateMusic);
    zox_sys_out(MusicSpeed);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Seed, seed);
        zox_sys_i(InstrumentType, instrument);
        zox_sys_o(GenerateMusic, generateMusic);
        zox_sys_o(MusicSpeed, speed);
        if (!generateMusic->value) {
            continue;
        }
        // NOTE: Clear old notes, they are children
        ordered_children old_notes = zox_get_ordered_children(world, e);
        for (int j = 0; j < old_notes.length; j++) {
            zox_delete(old_notes.value[j]);
        }
        static Song song;
        compose_song((uint) seed->value, instrument->value, &song);
        speed->value = song.step_seconds;
        for (uint step = 0; step < song.steps_count; step++) {
            const SongStep* song_step = &song.steps[step];
            const SongNote melody = song_step->voices[song_voice_melody];
            const entity note = spawn_note(world, prefab_note, e, melody.note, melody.instrument,
                melody.steps * song.step_seconds, melody.volume);
            for (byte v = 1; v < song_voices; v++) {
                const SongNote voice = song_step->voices[v];
                if (!voice.note) {
                    continue;
                }
                // held notes ring a little past their steps, capped at a sound's longest
                const float length = fminf(2.8f, voice.steps * song.step_seconds * 1.1f);
                spawn_note(world, prefab_note, note, voice.note, voice.instrument, length, voice.volume);
            }
        }
        zox_logv("Composed music [%s] seed [%lu]: [%u] steps, [%i] sections, mode [%i] tonic [%i]",
            zox_getn(e), seed->value, song.steps_count, song.sections_count, song.mode, song.tonic);
        generateMusic->value = 0;
    }
} zox_sys_end(MusicGenerateSystem);
