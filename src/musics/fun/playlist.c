void play_playlist(ecs *world,
    const entity realm,
    const byte new_index)
{
    if (!zox_valid(realm) || !zox_has(realm, PlaylistLinks)) {
        zox_log_error("invalid realm [%lu]", realm)
        return;
    }
    zox_geter(realm, PlaylistLinks, playlistLinks)
    if (!zox_valid(playlistLinks->length)) {
        zox_log_error("playlist has no music")
        return;
    }
    if (new_index >= playlistLinks->length) {
        zox_log_error("invalid index playlist [%i]", new_index)
        return;
    }
    zox_geter_value(realm, PlaylistPlaying, byte, old_index)
    if (new_index == old_index) {
        zox_log_error("playlist already playing track [%i]", old_index)
        return;
    }
    const entity old_playlist = playlistLinks->value[old_index];
    const entity new_playlist = playlistLinks->value[new_index];
    stop_music(world, old_playlist);
    play_music(world, new_playlist, 0);
    zox_set(realm, PlaylistPlaying, { new_index })
}