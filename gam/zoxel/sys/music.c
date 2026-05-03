zox_sys2(MusicRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_out(PlaylistLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GenerateRealm, state);
        zox_sys_o(PlaylistLinks, playlists);

        if (state->value != zox_generate_realm_music) {
            continue;
        }

        entity playlist_main_menu = spawn_playlist(world, prefab_playlist);
        add_to_PlaylistLinks(playlists, playlist_main_menu);
        MusicLinks musics_main_menu = (MusicLinks) { 0 };

        entity playlist_game = spawn_playlist(world, prefab_playlist);
        add_to_PlaylistLinks(playlists, playlist_game);
        MusicLinks musics_game = (MusicLinks) { 0 };

        // musics
        entity music_file = load_music_file(world, prefab_music_file, prefab_note, music_file_path);

        // add musics to playlists
        if (music_file) {
            // add_music(world, playlist_main_menu, music_file);
            add_to_MusicLinks(&musics_main_menu, music_file);
            zox_set(music_file, MusicEnabled, { 1 });
            local_music = music_file;
        } else {
            zox_log_error("File Loading Failed [%s]", music_file_path);

            entity main_menu_song = spawn_music_generated(
                world,
                prefab_music_generated,
                13666,
                instrument_edm,
                (float2) { 0.3f, 0.5f }
            );
            local_music = main_menu_song;
            // add_music(world, playlist_main_menu, main_menu_song);
            add_to_MusicLinks(&musics_main_menu, main_menu_song);
            zox_set(main_menu_song, MusicEnabled, { 1 });
        }

        entity generated_music = spawn_music_generated(
            world,
            prefab_music_generated,
            666,
            instrument_piano,
            (float2) { 1.0f, 1.4f }
        );

        add_to_MusicLinks(&musics_game, generated_music);
        // add_music(world, playlist_game, generated_music);

        zox_set_ptr(playlist_main_menu, MusicLinks, musics_main_menu);
        zox_set_ptr(playlist_game, MusicLinks, musics_game);

        zox_logv("At [%f] Realm [musics] [%i] spawned.", zox_current_time, playlists->length);
    }
} zox_sys_end(MusicRealmSpawnSystem);
