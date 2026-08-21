zox_tag(Note);
zox_tag(Music);
zox_tag(Looping);
zoxc_byte(MusicNote);
zoxc_byte(GenerateMusic);
zoxc_byte(MusicEnabled);
zoxc_double(MusicLength);
zoxc_double(MusicTime);
zoxc_double(MusicSpeed);
zoxc_entities(NoteLinks)
// playlist
zox_tag(Playlist);
zoxc_byte(PlaylistEnabled);
zoxc_byte(MusicPlaying);
zoxc_entities(MusicLinks)
// realm
zoxc_byte(PlaylistPlaying);
zoxc_entities(PlaylistLinks)
zoxc_entity(PlaylistLink);
// playlist mode
zoxc_byte(PlaylistMode);
// zoxc_arrayd(MusicData, int);

void zox_components_musics(ecs* world) {
    // Notes
    zoxd_tag(Note);
    zoxd_tag(Music);
    zoxd_tag(Looping);
    // Music
    zoxd_byte(MusicNote);
    zoxd_byte(GenerateMusic);
    zoxd_byte(MusicEnabled);
    zoxd_double(MusicTime);
    zoxd_double(MusicSpeed);
    zoxd_double(MusicLength);
    zoxd_entities(NoteLinks);
    // playlist
    zoxd_tag(Playlist);
    zoxd_byte(PlaylistEnabled);
    zoxd_byte(MusicPlaying);
    zoxd_entities(MusicLinks);
    // realm
    zoxd_byte(PlaylistPlaying);
    zoxd_byte(PlaylistMode);
    zoxd_entities(PlaylistLinks);
    zoxd_entity(PlaylistLink);
}