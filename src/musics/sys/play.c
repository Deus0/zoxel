zox_sys2(MusicPlaySystem) {
    if (nomusic || nosounds) {
        return;
    }

    float volume_music = get_volume_music();
    init_delta_time();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MusicEnabled);
    zox_sys_in(NoteLinks);
    zox_sys_in(MusicSpeed);
    zox_sys_out(MusicNote);
    zox_sys_out(MusicTime);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(MusicEnabled, playing);
        zox_sys_i(NoteLinks, notes);
        zox_sys_i(MusicSpeed, musicSpeed);
        zox_sys_o(MusicNote, musicNote);
        zox_sys_o(MusicTime, musicTime);

        if (!playing->value || !notes->length) {
            // zox_logw("Music disabled");
            continue;
        }

        musicTime->value += delta_time * global_music_speed;
        if (notes->length > 0 && musicTime->value >= musicSpeed->value) {
            musicTime->value -= musicSpeed->value;
            musicNote->value++;

            if (musicNote->value >= notes->length) {
                // zox_logw("Music disabled");
                musicNote->value = 0;
            }

            // music notes disabled atm
            if (!volume_music) {
                // zox_logw("Music disabled");
                continue;
            }

            entity note = notes->value[musicNote->value];
            int music_note = zox_getv(note, SoundFrequencyIndex);
            float note_volume = zox_getv(note, SoundVolume);
            float note_time = zox_getv(note, SoundLength);
            // const int music_note = musicData->value[musicNote->value];

            if (!music_note || !note_volume || !note_time) {
                continue;
            }

            float volume = note_volume * get_volume_music();
            zox_geter_value(note, InstrumentType, byte, note_instrument);
            float frequency = note_frequencies[music_note];
            // zox_log(" > spawning note type: %i\n", note_instrument)

            if (note_instrument == instrument_piano_file) {
                // spawn sound_file_note(world, prefab_sound_file_note)
                spawn_sound_from_file_name(world, prefab_sound, "piano", frequency, volume);
                zox_log("playing note.");
            } else {
                spawn_sound_generated(world,
                    prefab_sound_generated,
                    note_instrument,
                    frequency,
                    note_time,
                    volume);
                /*if (rand() % 100 >= music_play_double_chance) {
                    spawn_sound_generated(world,
                        prefab_sound_generated,
                        note_instrument,
                        frequency,
                        note_time,
                        volume);
                }*/
                // zox_log_notes("+ [%s] created [%lu]", zox_get_name(sound), sound)
            }

            // zox_log("+ playing [%lu:%s] at [%i]: (i%i:f%f) [note_volume [%f] - volume[%f]]", note, zox_get_name(note), musicNote->value, note_instrument, frequency, note_volume, volume)
            // zox_log_notes(" > music note played [%i : %i] frequency [%f] instrument [%i]", musicNote->value, music_note, frequency, instrumentType->value)
        }
    }
} zox_sys_end(MusicPlaySystem);
