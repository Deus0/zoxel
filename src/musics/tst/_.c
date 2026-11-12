// tst: music_file, music .zox files load, and save and load back the same
//  - the shipped res/music/music.zox loads its notes, each a valid length and pitch
//  - notes saved then loaded come back the same (a uint32 count, it was saved as an int
//      but loaded as a size_t, so the music never loaded)
//  - a file claiming more than MAX_NOTES notes is clamped (it used to overflow the notes array)
//  - run: bash bsh/tst.sh music_file
byte zox_tst_music_file_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_music_file_check(ecs* world) {
    (void) world;
    static MidiNote notes[MAX_NOTES];
    char* path = concat_file_path(resources_path, music_file_path);
    const int count = load_notes_from_file(notes, path);
    free(path);
    zox_log("[tst] res music has [%i] notes", count);
    zox_tst_check(count > 0, "res/music/music.zox loaded no notes");
    int invalid = 0;
    for (int i = 0; i < count; i++) {
        invalid += !(notes[i].length > 0 && notes[i].length < 2 && notes[i].frequency > 0);
    }
    zox_tst_check(invalid == 0, "[%i] of [%i] notes have an invalid length or pitch", invalid, count);
    // round trip
    const char* tmp_path = "../tst/music_roundtrip.zox";
    MidiNote saved[3] = {
        { .time = 0, .length = 0.5, .frequency = 220 },
        { .time = 0.5, .length = 0.25, .frequency = 330 },
        { .time = 0.75, .length = 1, .frequency = 440 },
    };
    zox_tst_check(save_notes_to_file(saved, 3, tmp_path) == 0, "couldn't save [%s]", tmp_path);
    static MidiNote loaded[MAX_NOTES];
    const int loaded_count = load_notes_from_file(loaded, tmp_path);
    zox_tst_check(loaded_count == 3, "saved [3] notes, loaded [%i]", loaded_count);
    zox_tst_check(loaded_count == 3 && !memcmp(saved, loaded, sizeof(saved)), "loaded notes differ from the saved ones");
    // a count past MAX_NOTES is clamped
    FILE* file = fopen(tmp_path, "wb");
    if (file) {
        const uint32_t too_many = MAX_NOTES * 4;
        fwrite(&too_many, sizeof(uint32_t), 1, file);
        static MidiNote filler[MAX_NOTES * 4];
        fwrite(filler, sizeof(MidiNote), MAX_NOTES * 4, file);
        fclose(file);
        const int clamped = load_notes_from_file(loaded, tmp_path);
        zox_tst_check(clamped == MAX_NOTES, "a file of [%i] notes loaded [%i], expected [%i]", MAX_NOTES * 4, clamped, MAX_NOTES);
    }
    remove(tmp_path);
}

// tst: music_plays, the main menu's music plays: it steps through its notes, spawning sounds
//  - turns sounds back on (--background turns them off for tests)
//  - run: bash bsh/tst.sh music_plays
int zox_tst_music_plays_frames = 0;
int zox_tst_music_plays_start_sounds = 0;
byte zox_tst_music_plays_start_note = 0;
double zox_tst_music_plays_start = 0;
entity zox_tst_music_plays_music = 0;

byte zox_tst_music_plays_ready(ecs* world) {
    zox_tst_music_plays_frames++;
    // tests run --background, which turns sounds off, this one turns them on (no audio device plays them)
    nosounds = 0;
    zox_tst_waiting = "the menu music to start";
    const entity game = local_game;
    const entity music = zox_valid(game) ? zox_get_link(world, game, MenuMusicLink) : 0;
    if (!zox_valid(music) || !zox_getv(music, MusicEnabled)) {
        return 0;
    }
    if (!zox_tst_music_plays_music) {
        zox_tst_music_plays_music = music;
        zox_tst_music_plays_start = zox_current_time;
        zox_tst_music_plays_start_note = zox_getv(music, MusicNote);
        zox_tst_music_plays_start_sounds = zox_count_types(Sound);
    }
    zox_tst_waiting = "a few seconds of music";
    return zox_current_time - zox_tst_music_plays_start > 3.0;
}

void zox_tst_music_plays_check(ecs* world) {
    const entity music = zox_tst_music_plays_music;
    const byte note = zox_getv(music, MusicNote);
    const int sounds = zox_count_types(Sound);
    zox_log("[tst] menu music at note [%i] (from [%i]), sounds [%i] (from [%i])",
        note, zox_tst_music_plays_start_note, sounds, zox_tst_music_plays_start_sounds);
    zox_tst_check(note != zox_tst_music_plays_start_note, "the menu music didn't move on from note [%i] in 3 seconds", note);
    zox_tst_check(zox_get_children_count(world, music) > 0, "the menu music has no notes");
    zox_tst_check(sounds > zox_tst_music_plays_start_sounds, "the menu music spawned no sounds");
}

void zox_tsts_musics() {
    zox_tst_add("music_file", zox_tst_music_file_ready, zox_tst_music_file_check, 30);
    zox_tst_set_setup("music_file", NULL, 1);
    zox_tst_add("music_plays", zox_tst_music_plays_ready, zox_tst_music_plays_check, 60);
    zox_tst_set_setup("music_plays", NULL, 1);
}
