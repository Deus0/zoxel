// tst: music_theme, the menu theme is new each time but always has its call
//  - for 20 seeds: the call (theme_call) is played by the lead in every main and rise section, at its steps
//  - its notes are in order, inside the theme, under the events cap, it lasts a minute and a half to three
//  - the same seed composes the same theme, seeds vary in key, mode, tempo, form and the answer
//  - as loud as the realm's song (seed 666), the menu and the game crossfade at one volume
//  - writes tst/menu_theme.wav (seed 1, the mix it measured), to listen to; with zox_tst_listen=1 also
//    tst/menu_theme2.wav (seed 2), rendered only then as nothing checks it (a song is ~2s to synthesize in debug)
//  - run: bash bsh/tst.sh music_theme
byte zox_tst_music_theme_ready(ecs* world) {
    (void) world;
    return 1;
}

// is there a lead note at a start (seconds)
static inline byte zox_tst_theme_has(const Song* song, float start, int note, byte instrument) {
    for (uint i = 0; i < song->events_count; i++) {
        const SongEvent event = song->events[i];
        if (fabsf(event.start - start) < 0.002f && event.note == note && event.instrument == instrument) {
            return 1;
        }
    }
    return 0;
}

static inline double zox_tst_theme_rms(const float* samples, uint length) {
    double power = 0;
    for (uint j = 0; j < length; j++) {
        power += samples[j] * samples[j];
    }
    return length ? sqrt(power / length) : 0;
}

void zox_tst_music_theme_check(ecs* world) {
    (void) world;
    static Song song;
    static Song again;
    byte tonics[notes_max] = { 0 };
    byte modes[theme_modes_count] = { 0 };
    byte forms[song_max_sections * 2 + 2] = { 0 };
    uint melodies[20];
    int tempos = 0;
    float last_step = 0;
    for (uint seed = 1; seed <= 20; seed++) {
        compose_menu_theme(seed, &song);
        tonics[song.tonic] = 1;
        modes[song.mode] = 1;
        forms[song.sections_count * 2 + (song.sections[1].role == theme_role_echo)] = 1;
        tempos += fabsf(song.step_seconds - last_step) > 0.001f;
        last_step = song.step_seconds;
        zox_tst_check(song.seconds >= 90 && song.seconds <= 180, "seed [%u] lasts [%.0fs]", seed, song.seconds);
        zox_tst_check(song.events_count > 500 && song.events_count < song_max_events, "seed [%u] has [%u] notes, the cap is [%i]",
            seed, song.events_count, song_max_events);
        int unsorted = 0, outside = 0, calls = 0, calls_missing = 0;
        uint melody = 2166136261u;
        for (uint i = 0; i < song.events_count; i++) {
            const SongEvent event = song.events[i];
            unsorted += i > 0 && event.start < song.events[i - 1].start;
            outside += event.start < 0 || event.start >= song.seconds || event.seconds <= 0 || event.seconds > song_max_note_seconds;
            if (event.instrument == instrument_synth_lead) {
                melody = (melody ^ (uint) (event.note - song.tonic + 64)) * 16777619u;
            }
        }
        melodies[seed - 1] = melody;
        // the call, in each main and rise section, twice (bars 0 and 4)
        for (int s = 0; s < song.sections_count; s++) {
            const SongSection section = song.sections[s];
            if (section.role != theme_role_main && section.role != theme_role_rise) {
                continue;
            }
            for (int half = 0; half < 2; half++) {
                for (int n = 0; n < theme_call_count; n++) {
                    const ThemeNote note = theme_call[n];
                    const float start = ((section.start_bar + half * 4) * theme_steps_per_bar + note.step) * song.step_seconds;
                    calls++;
                    calls_missing += !zox_tst_theme_has(&song, start, song.tonic + note.note, instrument_synth_lead);
                }
            }
        }
        zox_tst_check(unsorted == 0, "seed [%u] has [%i] notes out of order", seed, unsorted);
        zox_tst_check(outside == 0, "seed [%u] has [%i] notes outside the theme or too long", seed, outside);
        zox_tst_check(calls >= 2 * theme_call_count * 2, "seed [%u] plays the call [%i] notes, in too few sections", seed, calls);
        zox_tst_check(calls_missing == 0, "seed [%u] misses [%i] of the call's [%i] notes", seed, calls_missing, calls);
        compose_menu_theme(seed, &again);
        zox_tst_check(!memcmp(&song, &again, sizeof(Song)), "seed [%u] composed a different theme the second time", seed);
    }
    int tonic_count = 0, mode_count = 0, form_count = 0, melody_count = 0;
    for (int i = 0; i < notes_max; i++) {
        tonic_count += tonics[i];
    }
    for (int i = 0; i < theme_modes_count; i++) {
        mode_count += modes[i];
    }
    for (int i = 0; i < song_max_sections * 2 + 2; i++) {
        form_count += forms[i];
    }
    for (int i = 0; i < 20; i++) {
        byte seen = 0;
        for (int j = 0; j < i; j++) {
            seen |= melodies[j] == melodies[i];
        }
        melody_count += !seen;
    }
    zox_log("[tst] 20 themes: [%i] keys, [%i] modes, [%i] forms, [%i] melodies, [%i] tempo changes",
        tonic_count, mode_count, form_count, melody_count, tempos);
    zox_tst_check(tonic_count >= 4, "20 themes only used [%i] keys", tonic_count);
    zox_tst_check(mode_count >= 2, "20 themes only used [%i] modes", mode_count);
    zox_tst_check(form_count >= 2, "20 themes only used [%i] floor plans", form_count);
    zox_tst_check(melody_count >= 15, "20 themes only had [%i] different melodies (their answers)", melody_count);
    zox_tst_check(tempos >= 15, "20 themes only changed tempo [%i] times", tempos);
    // as loud as the realm's song
    static Song realm;
    compose_song(666, instrument_soft_piano, &realm);
    compose_menu_theme(1, &song);
    uint theme_length, realm_length;
    float* theme_mix = zox_tst_mix_song(&song, 1, &theme_length);
    float* realm_mix = zox_tst_mix_song(&realm, 1, &realm_length);
    const double theme_rms = zox_tst_theme_rms(theme_mix, theme_length);
    const double realm_rms = zox_tst_theme_rms(realm_mix, realm_length);
    const float ratio = realm_rms > 0 ? (float) (theme_rms / realm_rms) : 0;
    zox_log("[tst] theme [1]: [%.0fs], [%u] notes, tonic [%i], mode [%i], [%i] sections, rms [%.3f], the realm song's [%.3f] ([%.2fx])",
        song.seconds, song.events_count, song.tonic, song.mode, song.sections_count, theme_rms, realm_rms, ratio);
    zox_tst_check(ratio > 0.7f && ratio < 1.4f, "the menu theme is [%.2fx] as loud as the realm's song", ratio);
    zox_tst_write_song(theme_mix, theme_length, zox_tst_path("menu_theme.wav"));
    free(theme_mix);
    free(realm_mix);
    const char* listen = getenv("zox_tst_listen");
    if (listen && listen[0] == '1') {
        compose_menu_theme(2, &song);
        zox_tst_render_song(&song, zox_tst_path("menu_theme2.wav"));
    }
}
