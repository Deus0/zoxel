void stop_music(ecs *world, entity playlist) {
    if (!playlist || !zox_has(playlist, MusicPlaying)) {
        zox_loge("invalid playlist in [play_music]")
        return;
    }
    byte old_index = zox_getv(playlist, MusicPlaying);
    const MusicLinks* musicLinks = zox_get(playlist, MusicLinks);
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
        zox_loge("invalid playlist in [play_music]");
        return;
    }
    byte old_index = zox_getv(playlist, MusicPlaying);
    // set previous music off and new one on
    const MusicLinks* musics = zox_get(playlist, MusicLinks);
    if (new_index >= musics->length) {
        zox_loge("new_index out of bounds [%i] >= [%i]", new_index, musics->length);
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
        zox_loge("cannot add music to null playlist");
        return;
    }
    // zox_log(" + added music to playlist [%lu]\n", music)
    zox_muter(playlist, MusicLinks, musicLinks);
    add_to_MusicLinks(musicLinks, music);
}

// Fades from the music holder is playing to music, over music_fade_seconds (music_fade_system)
//  - holder is the game, it plays its menu music in the menus and the realm's music in game
void crossfade_music(
    ecs* world,
    entity holder,
    entity music)
{
    if (!zox_valid(holder)) {
        return;
    }
    const entity old = zox_get_link(world, holder, MusicPlayingLink);
    if (old == music) {
        return;
    }
    if (zox_valid(old) && zox_has(old, MusicVolumeTarget)) {
        zox_setv(old, MusicVolumeTarget, 0);
    }
    if (!zox_valid(music)) {
        zox_set_link(world, holder, MusicPlayingLink, 0);
        return;
    }
    // NOTE: a music spawned this frame has no components yet (deferred), so no zox_has checks
    const byte is_playing = zox_has(music, MusicEnabled) && zox_getv(music, MusicEnabled);
    if (!is_playing) {
        zox_setv(music, MusicVolume, 0);
        zox_setv(music, MusicEnabled, 1);
    }
    zox_setv(music, MusicVolumeTarget, 1);
    zox_set_link(world, holder, MusicPlayingLink, music);
}

// the game's main menu music
void play_menu_music(ecs* world, entity game) {
    crossfade_music(world, game, zox_get_link(world, game, MenuMusicLink));
}

// a track of the realm's in game music (its first playlist's): 0 the overworld, 1 in towns (gam/zoxel/sys/music.c),
//  its first if it has no such track
void play_realm_track(ecs* world, entity realm, byte track) {
    const entity game = zox_get_link(world, realm, GameLink);
    const entity playlist = zox_get_child_by_id(world, realm, zox_id(Playlist));
    if (!zox_valid(game) || !zox_valid(playlist)) {
        zox_loge("[play_realm_track] realm [%s] has no game or playlist", zox_getn(realm));
        return;
    }
    const MusicLinks* musics = zox_get(playlist, MusicLinks);
    track = track < musics->length ? track : 0;
    crossfade_music(world, game, musics->length ? musics->value[track] : 0);
}

// the realm's overworld music
void play_realm_music(ecs* world, entity realm) {
    play_realm_track(world, realm, 0);
}
