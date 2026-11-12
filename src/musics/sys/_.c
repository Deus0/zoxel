#include "play.c"
#include "generate.c"

void zox_systems_music(ecs* world) {
    zox_system_1(
        music_generate_system,
        zoxp_spawn,
        [in] core.Seed,
        [in] sounds.InstrumentType,
        [out] musics.GenerateMusic,
        [out] musics.MusicSpeed,
        [none] musics.Music
    );
    zox_system_1(
        music_play_system,
        zoxp_spawn,
        [in] musics.MusicEnabled,
        [out] musics.MusicNote,
        [out] musics.MusicTime,
        [none] musics.Music
    );
    zox_system(
        music_fade_system,
        zoxp_update,
        [in] musics.MusicVolumeTarget,
        [out] musics.MusicVolume,
        [out] musics.MusicEnabled,
        [none] musics.Music
    );
}
