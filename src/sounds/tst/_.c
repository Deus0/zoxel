// tst: sound_formats, sounds are mono 44100 hz, the format every play path uses
//  - every loaded sound file is mono: its samples are its length in seconds * 44100
//      (stereo wavs are downmixed, they were kept interleaved and played as stereo, while
//      mono ones played as stereo too, at double speed)
//  - a generated sound's data is its length in seconds * 44100
//  - run: bash bsh/tst.sh sound_formats
int zox_tst_sound_frames = 0;
entity zox_tst_sound_generated = 0;

byte zox_tst_sound_formats_ready(ecs* world) {
    zox_tst_sound_frames++;
    if (zox_tst_sound_frames == 5) {
        zox_tst_sound_generated = spawn_sound_generated(world, prefab_sound_generated, instrument_piano, 440, 0.5f, 0.5f);
    }
    zox_tst_waiting = "a few frames";
    return zox_tst_sound_frames >= 10;
}

void zox_tst_sound_formats_check(ecs* world) {
    int checked = 0;
    for (int i = 0; i < sound_files_count; i++) {
        const entity e = files_sounds[i];
        if (!zox_valid(e) || !zox_has(e, SoundData) || !zox_has(e, SoundLength)) {
            continue;
        }
        const SoundData* data = zox_get(e, SoundData);
        const float seconds = zox_getv(e, SoundLength);
        const int expected = (int) (seconds * sound_sample_rate + 0.5f);
        // MAX_SOUND_DATA cuts long sounds
        const int expected_cut = expected < MAX_SOUND_DATA ? expected : MAX_SOUND_DATA;
        zox_tst_check(abs((int) data->length - expected_cut) <= 1,
            "sound file [%s] has [%u] samples for [%.3fs], expected [%i] (mono 44100)",
            zox_getn(e), data->length, seconds, expected_cut);
        checked++;
    }
    zox_log("[tst] checked [%i] sound files", checked);
    zox_tst_check(checked > 0, "no sound files loaded");
    zox_tst_check(zox_valid(zox_tst_sound_generated), "the generated sound wasn't spawned");
    if (zox_valid(zox_tst_sound_generated)) {
        const uint length = zox_get(zox_tst_sound_generated, SoundData)->length;
        zox_tst_check(length == (uint) (0.5f * sound_sample_rate), "the generated sound has [%u] samples for 0.5s", length);
    }
}

void zox_tsts_sounds() {
    zox_tst_add("sound_formats", zox_tst_sound_formats_ready, zox_tst_sound_formats_check, 30);
    zox_tst_set_setup("sound_formats", NULL, 1);
}
