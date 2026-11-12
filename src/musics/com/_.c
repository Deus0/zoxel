zox_tag(Note);
zoxc_double(NoteStart);     // seconds from its music's start
zox_tag(Music);
zox_tag(Looping);
zoxc_uint(MusicNote);        // the next note to play
zoxc_byte(GenerateMusic);
zoxc_byte(MusicEnabled);
zoxc_double(MusicLength);    // seconds, then it loops (0: when its last note ends)
zoxc_double(MusicTime);      // the playhead, seconds from its start
zoxc_double(MusicSpeed);     // seconds per step, its tempo
// crossfading: the volume eases to its target, and switches the music off once it's silent
zoxc_float(MusicVolume);
zoxc_float(MusicVolumeTarget);
zox_tag(MusicPlayingLink);  // holder (the game) -> the music it's playing
zox_tag(MenuMusicLink);     // game -> its main menu music (the realm's music plays in game)
// playlist
zox_tag(Playlist);
zoxc_byte(PlaylistEnabled);
zoxc_byte(MusicPlaying);
zoxc_entities(MusicLinks)
// playlist mode
zoxc_byte(PlaylistMode);
// zoxc_arrayd(MusicData, int);

void zox_components_musics(ecs* world) {
    // Notes
    zoxd_tag(Note);
    zoxd_double(NoteStart);
    zoxd_tag(Music);
    zoxd_tag(Looping);
    // Music
    zoxd_uint(MusicNote);
    zoxd_byte(GenerateMusic);
    zoxd_byte(MusicEnabled);
    zoxd_double(MusicTime);
    zoxd_double(MusicSpeed);
    zoxd_double(MusicLength);
    zoxd_float(MusicVolume);
    zoxd_float(MusicVolumeTarget);
    zoxd_nf_link(MusicPlayingLink);
    zoxd_nf_link(MenuMusicLink);
    // playlist
    zoxd_tag(Playlist);
    zoxd_byte(PlaylistEnabled);
    zoxd_byte(MusicPlaying);
    zoxd_entities(MusicLinks);
    // realm
    zoxd_byte(PlaylistMode);
}
