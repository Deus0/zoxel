entity spawn_prefab_playlist(ecs *world) {
    zox_prefab();
    zox_prefab_name("playlist");
    zox_add(e, Playlist);
    zox_setv(e, PlaylistMode, playlist_mode_loop);
    zox_setv(e, MusicPlaying, 0);
    zox_add(e, MusicLinks);
    return e;
}

entity spawn_playlist(ecs *world, entity parent) {
    zox_instance(prefab_playlist);
    zox_name("playlist");
    zox_set_parent(world, e, parent);
    return e;
}
