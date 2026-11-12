// Generates a music's notes from its seed (compose_song, like a dungeon from its seed)
//  - its notes are the performed song's events, each with its own start and length (they overlap)
//  - its lead instrument is the music's InstrumentType, it loops after the song's length
void music_generate_system(iter* it) {
    zox_sys_on_begin();
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
        zox_set(e, MusicLength, { song.seconds });
        zox_set(e, MusicTime, { 0 });
        zox_set(e, MusicNote, { 0 });
        for (uint j = 0; j < song.events_count; j++) {
            const SongEvent event = song.events[j];
            spawn_note(world, prefab_note, e, event.note, event.instrument, event.start, event.seconds, event.volume);
        }
        zox_logv("Composed music [%s] seed [%lu]: [%u] notes over [%.0fs], [%i] sections, mode [%i] tonic [%i]",
            zox_getn(e), seed->value, song.events_count, song.seconds, song.sections_count, song.mode, song.tonic);
        generateMusic->value = 0;
    }
    zox_sys_on_end();
} zoxd_system(music_generate_system);
