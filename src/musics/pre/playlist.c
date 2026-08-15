entity spawn_prefab_playlist(ecs *world) {
    zox_prefab();
    zox_prefab_name("playlist");
    zox_add(e, Playlist);
    zox_prefab_set(e, PlaylistMode, { playlist_mode_loop });
    zox_prefab_set(e, MusicPlaying, { 0 });
    zox_prefab_add(e, MusicLinks);
    return e;
}

entity spawn_playlist(ecs *world, const entity prefab) {
    zox_instance(prefab);
    zox_name("playlist");
    return e;
}
