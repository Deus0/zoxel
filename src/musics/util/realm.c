void spawn_realm_playlist(ecs *world, const entity realm) {
    if (!realm) {
        zox_log_error("realm invalid")
        return;
    }
    // add them to realm
    if (!zox_has(realm, PlaylistLinks)) {
        zox_log_error("realm [%lu] has no PlaylistLinks", realm);
        return;
    }
    // zox_muter(realm, PlaylistLinks, playlistLinks)
    PlaylistLinks playlistLinks2 = { 0, NULL };
    PlaylistLinks *playlistLinks = &playlistLinks2;
    const entity playlist_main_menu = spawn_playlist(world, prefab_playlist);
    const entity playlist_game = spawn_playlist(world, prefab_playlist);
    add_to_PlaylistLinks(playlistLinks, playlist_main_menu);
    add_to_PlaylistLinks(playlistLinks, playlist_game);
    // musics
    entity music_file = load_music_file(
        world,
        prefab_music_file,
        prefab_note,
        music_file_path
    );
    // add musics to playlists
    if (music_file) {
        add_music(world, playlist_main_menu, music_file);
        zox_set(music_file, MusicEnabled, { 1 });
        local_music = music_file;
    } else {
        zox_log_error("File Loading Failed [%s]", music_file_path);

        const entity main_menu_song = spawn_music_generated(
            world,
            prefab_music_generated,
            13666,
            instrument_edm,
            (float2) { 0.3f, 0.5f }
        );
        local_music = main_menu_song;
        add_music(world, playlist_main_menu, main_menu_song);
        zox_set(main_menu_song, MusicEnabled, { 1 });
    }

    const entity generated_music = spawn_music_generated(
        world,
        prefab_music_generated,
        666,
        instrument_piano,
        (float2) { 1.0f, 1.4f }
    );

    add_music(world, playlist_game, generated_music);

    zox_set(realm, PlaylistLinks, {
        .length = playlistLinks->length,
        .value = playlistLinks->value
    });
}