#include "music_play_system.c"
#include "music_generate_system.c"
#include "realm.c"
realm_clear_system(PlaylistLinks);


void define_systems_music(ecs* world) {
    realm_clear_systemd(musics, PlaylistLinks);

    zox_system_1(
        MusicGenerateSystem,
        zoxp_mainthread,
        [in] core.Seed,
        [in] sounds.InstrumentType,
        [out] GenerateMusic,
        [out] NoteLinks,
        [none] Music
    );
    zox_system_1(
        MusicPlaySystem,
        zoxp_mainthread,
        [in] MusicEnabled,
        [in] NoteLinks,
        [in] MusicSpeed,
        [out] MusicNote,
        [out] MusicTime,
        [none] Music
    );
    zox_system_1(
        MusicRealmSpawnSystem,
        zoxp_mainthread,
        [in] realms.GenerateRealm,
        [out] musics.PlaylistLinks,
        [none] realms.Realm
    );
}
