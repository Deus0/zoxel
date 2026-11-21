// tst: sound_trigger, a sound generated plays: GenerateSound, then its Dirty and Update (the frame after) trigger it,
//  sound_play_system hands it to the mixer
//  - the mixer runs without an audio device (tests run --background), so the test turns sounds and audio on
//  - run: bash bsh/tst.sh sound_trigger
int zox_tst_sound_trigger_frames = 0;
entity zox_tst_sound_trigger_sound = 0;
byte zox_tst_sound_trigger_updated = 0;
int zox_tst_sound_trigger_voices = 0;

byte zox_tst_sound_trigger_ready(ecs* world) {
    zox_tst_progress(++zox_tst_sound_trigger_frames);
    nosounds = 0;
    audio_enabled = 1;
    zox_tst_waiting = "the clock past a second (no sounds before)";
    if (zox_current_time <= 1.5) {
        return zox_tst_sound_trigger_frames > 2000;
    }
    if (!zox_tst_sound_trigger_sound) {
        zox_mixer_stop_all();
        zox_tst_sound_trigger_sound = spawn_sound_generated(world, prefab_sound_generated, 0, 440, 2, 0.5f);
        zox_tst_sound_trigger_frames = 0;
        return 0;
    }
    zox_tst_waiting = "the sound in the mixer";
    if (zox_valid(zox_tst_sound_trigger_sound) && zox_has(zox_tst_sound_trigger_sound, Update)) {
        zox_tst_sound_trigger_updated = 1;
    }
    zox_tst_sound_trigger_voices = zox_mixer_voices_playing();
    return (zox_tst_sound_trigger_updated && zox_tst_sound_trigger_voices > 0) || zox_tst_sound_trigger_frames > 120;
}

void zox_tst_sound_trigger_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_sound_trigger_sound, "no sound spawned");
    zox_tst_check(zox_tst_sound_trigger_updated, "the sound was never triggered (no Update)");
    zox_tst_check(zox_tst_sound_trigger_voices > 0, "the mixer plays nothing [%i]", zox_tst_sound_trigger_voices);
}

void zox_tsts_sound_trigger() {
    zox_tst_add("sound_trigger", zox_tst_sound_trigger_ready, zox_tst_sound_trigger_check, 30);
    zox_tst_set_setup("sound_trigger", NULL, 1);
}
