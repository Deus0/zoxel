#include "play.c"
#include "generate.c"

void zox_systems_music(ecs* world) {
    zox_system_1(
        MusicGenerateSystem,
        zoxp_spawn,
        [in] core.Seed,
        [in] sounds.InstrumentType,
        [out] musics.GenerateMusic,
        [out] musics.MusicSpeed,
        [none] musics.Music
    );
    zox_system_1(
        MusicPlaySystem,
        zoxp_spawn,
        [in] musics.MusicEnabled,
        [in] musics.MusicSpeed,
        [out] musics.MusicNote,
        [out] musics.MusicTime,
        [none] musics.Music
    );
    zox_system(
        MusicFadeSystem,
        zoxp_update,
        [in] musics.MusicVolumeTarget,
        [out] musics.MusicVolume,
        [out] musics.MusicEnabled,
        [none] musics.Music
    );
}
