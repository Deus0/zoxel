#include "play.c"
#include "generate.c"
realm_clear_system(PlaylistLinks);

void zox_systems_music(ecs* world) {
    realm_clear_systemd(musics, PlaylistLinks);
    zox_system_1(
        MusicGenerateSystem,
        zoxp_mainthread,
        [in] core.Seed,
        [in] sounds.InstrumentType,
        [out] musics.GenerateMusic,
        [out] musics.NoteLinks,
        [none] musics.Music
    );
    zox_system_1(
        MusicPlaySystem,
        zoxp_mainthread,
        [in] musics.MusicEnabled,
        [in] musics.NoteLinks,
        [in] musics.MusicSpeed,
        [out] musics.MusicNote,
        [out] musics.MusicTime,
        [none] musics.Music
    );
}
