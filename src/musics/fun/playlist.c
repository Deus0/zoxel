void play_playlist(
    ecs *world,
    entity realm,
    byte new_index)
{
    if (!zox_valid(realm)) {
        zox_log_error("play_playlist invalid realm [%lu]", realm);
        return;
    }
    entity playlists[64];
    uint playlists_length = zox_get_children_by_id(
        world,
        realm,
        playlists,
        64,
        zox_id(Playlist));
    if (!playlists_length) {
        zox_loge("playlist has no music");
        return;
    }
    if (new_index >= playlists_length) {
        zox_loge("invalid index playlist [%i]", new_index);
        return;
    }
    entity playing_playlist = zox_get_link(world, realm, Playlist);
    entity new_playlist = playlists[new_index];
    if (new_playlist == playing_playlist) {
        zox_loge("playlist already playing track [%s]",
            zox_getn(playing_playlist));
        return;
    }
    if (zox_valid(playing_playlist)) {
        stop_music(world, playing_playlist);
        zox_unlink(world, realm, Playlist, playing_playlist);
    }
    play_music(world, new_playlist, 0);
    zox_link(world, realm, Playlist, new_playlist);
}
