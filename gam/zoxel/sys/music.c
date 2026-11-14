// The realm's in game music (generated): the overworld's (track 0), played when the player's character spawns, and
//  its towns' (track 1), played inside one (town_music_entered)
//  - the main menu's music is on the game (spawn_menu_music), it lasts from boot
void music_realm_spawn_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        if (state->value != zox_generate_realm_music) {
            continue;
        }
        entity playlist_game = spawn_playlist(world, e);
        MusicLinks musics_game = (MusicLinks) { 0 };
        entity generated_music = spawn_music_generated(
            world,
            prefab_music_generated,
            666,
            instrument_soft_piano,
            (float2) { 1.0f, 1.4f }
        );
        add_to_MusicLinks(&musics_game, generated_music);
        // in towns (track 1, town_music_entered): its own tune, a calmer flute
        add_to_MusicLinks(&musics_game, spawn_music_generated(world, prefab_music_generated, 4242, instrument_flute,
            (float2) { 0.8f, 1.0f }));
        zox_set_ptr(playlist_game, MusicLinks, musics_game);
        zox_logv("Realm [musics] [X] spawned.");
    }
    zox_sys_on_end();
} zoxd_system(music_realm_spawn_system);

// The main menu's music, on the game: the imported song (res/music, make convert), or a
// generated one if it won't load, it crossfades with the realm's music (crossfade_music)
void spawn_menu_music(ecs* world, entity app) {
    const entity game = zox_valid(app) ? zox_get_child_by_id(world, app, zox_id(Game)) : 0;
    if (!zox_valid(game)) {
        zox_loge("[spawn_menu_music] Invalid [game]");
        return;
    }
    entity music = load_music_file(
        world,
        prefab_music_file,
        prefab_note,
        music_file_path);
    if (!music) {
        zox_loge("File Loading Failed [%s]", music_file_path);
        music = spawn_music_generated(
            world,
            prefab_music_generated,
            13666,
            instrument_harp,
            (float2) { 0.3f, 0.5f });
    }
    zox_set_parent(world, music, game);
    zox_set_link(world, game, MenuMusicLink, music);
    local_music = music;
    // the link above is deferred, so crossfades to the music itself
    crossfade_music(world, game, music);
}

// a player's character entering a town (InTown, in_town_system) plays the town's music, leaving it the overworld's
void town_music_entered(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        play_realm_track(world, zox_get_link(world, e, RealmLink), 1);
    }
}

void town_music_left(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        if (!zox_has(e, Dead)) {
            play_realm_track(world, zox_get_link(world, e, RealmLink), 0);
        }
    }
}
