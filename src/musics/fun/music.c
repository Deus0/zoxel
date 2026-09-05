void stop_music(ecs *world, entity playlist) {
    if (!playlist || !zox_has(playlist, MusicPlaying)) {
        zox_loge("invalid playlist in [play_music]")
        return;
    }
    byte old_index = zox_getv(playlist, MusicPlaying);
    zox_geter(playlist, MusicLinks, musicLinks);
    if (old_index >= musicLinks->length) {
        zox_loge("old_index out of bounds [%i] >= [%i]", old_index, musicLinks->length)
        return;
    }
    zox_set(musicLinks->value[old_index], MusicEnabled, { 0 });
    if (is_log_playlists) {
        zox_log("- playlist stopped track [%i]", old_index);
    }
}

void play_music(ecs *world, entity playlist, byte new_index) {
    if (!playlist || !zox_has(playlist, MusicPlaying)) {
        zox_log_error("invalid playlist in [play_music]");
        return;
    }
    byte old_index = zox_getv(playlist, MusicPlaying);
    // set previous music off and new one on
    zox_geter(playlist, MusicLinks, musics);
    if (new_index >= musics->length) {
        zox_log_error("new_index out of bounds [%i] >= [%i]", new_index, musics->length);
        return;
    }
    zox_set(musics->value[old_index], MusicEnabled, { 0 });
    zox_set(musics->value[new_index], MusicEnabled, { 1 });
    zox_set(playlist, MusicPlaying, { new_index });
    if (is_log_playlists) {
        zox_log("+ playlist playing new track [%i] -> [%i]",
            old_index,
            new_index);
    }
}

void add_music(ecs *world, entity playlist, entity music) {
    if (!playlist) {
        zox_log_error("cannot add music to null playlist");
        return;
    }
    // zox_log(" + added music to playlist [%lu]\n", music)
    zox_muter(playlist, MusicLinks, musicLinks);
    add_to_MusicLinks(musicLinks, music);
}
