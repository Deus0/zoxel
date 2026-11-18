// tst: gpu_restore_game, a gpu dispose and restore in a game (chunks, lod meshes, characters' skeletons, particles)
//  - live gpu objects (zox_tst_gr_live): none after a dispose, as many as before after a restore in the same frame
//  - while disposed (android's lost context, no gpu to make things on) nothing makes a gpu object
//  - restored, no new opengl errors over the next frames (ones from before are left to their own bug)
//  - run: bash bsh/tst.sh gpu_restore_game
int zox_tst_grg_stage = 0, zox_tst_grg_frames = 0;
double zox_tst_grg_time = 0;
int zox_tst_grg_before[zox_tst_gr_kinds], zox_tst_grg_disposed[zox_tst_gr_kinds], zox_tst_grg_restored[zox_tst_gr_kinds];
int zox_tst_grg_made[zox_tst_gr_kinds];     // the most made while disposed, over its frames
int zox_tst_grg_errors = 0, zox_tst_grg_errors_before = 0;    // opengl errors seen (zox_gpu_error_bits)

byte zox_tst_gpu_restore_game_ready(ecs* world) {
    zox_tst_progress(zox_tst_grg_stage * 1000 + zox_tst_grg_frames);
    if (zox_tst_grg_stage == 0) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_waiting = "the game to settle";
        if (!zox_tst_grg_time) {
            zox_tst_grg_time = zox_current_time;
        }
        if (zox_current_time - zox_tst_grg_time < 3) {
            zox_tst_grg_errors_before |= zox_gpu_error_bits();
            return 0;
        }
        // in one frame: nothing else makes or lets go of anything between
        zox_tst_gr_live(zox_tst_grg_before);
        opengl_dispose_resources(world);
        zox_tst_gr_live(zox_tst_grg_disposed);
        opengl_restore_resources(world);
        zox_tst_gr_live(zox_tst_grg_restored);
        zox_tst_grg_errors |= zox_gpu_error_bits();
        zox_tst_grg_stage = 3;
        zox_tst_grg_frames = 0;
        return 0;
    }
    zox_tst_grg_frames++;
    if (zox_tst_grg_stage == 1) {
        zox_tst_waiting = "frames disposed";
        int live[zox_tst_gr_kinds];
        zox_tst_gr_live(live);
        for (int i = 0; i < zox_tst_gr_kinds; i++) {
            if (live[i] > zox_tst_grg_made[i]) {
                zox_tst_grg_made[i] = live[i];
            }
        }
        if (zox_tst_grg_frames < 60) {
            return 0;
        }
        opengl_restore_resources(world);
        zox_tst_grg_stage = 2;
        zox_tst_grg_frames = 0;
        return 0;
    }
    zox_tst_waiting = "frames restored";
    zox_tst_grg_errors |= zox_gpu_error_bits();
    if (zox_tst_grg_stage == 3 && zox_tst_grg_frames >= 60) {
        // then disposed for a while
        opengl_dispose_resources(world);
        zox_tst_grg_stage = 1;
        zox_tst_grg_frames = 0;
        return 0;
    }
    return zox_tst_grg_stage == 2 && zox_tst_grg_frames >= 60;
}

void zox_tst_gpu_restore_game_check(ecs* world) {
    (void) world;
    for (int i = 0; i < zox_tst_gr_kinds; i++) {
        const char* kind = zox_tst_gr_kind_names[i];
        zox_log("[tst] [metric] live %s: [%i] before, [%i] disposed, [%i] restored, [%i] made while disposed", kind,
            zox_tst_grg_before[i], zox_tst_grg_disposed[i], zox_tst_grg_restored[i], zox_tst_grg_made[i]);
        zox_tst_check(zox_tst_grg_disposed[i] == 0, "disposed, [%i] %s still live", zox_tst_grg_disposed[i], kind);
        zox_tst_check(zox_tst_grg_restored[i] == zox_tst_grg_before[i], "restored, [%i] %s live, [%i] before",
            zox_tst_grg_restored[i], kind, zox_tst_grg_before[i]);
        zox_tst_check(zox_tst_grg_made[i] == 0, "[%i] %s made while disposed (no gpu on android then)", zox_tst_grg_made[i], kind);
    }
    zox_tst_check(zox_tst_grg_before[0] > 0, "nothing on the gpu to test");
    // ones it had before are another bug (doc/todo.md), not the restore's
    zox_tst_check(!(zox_tst_grg_errors & ~zox_tst_grg_errors_before), "opengl errors after restoring, bits [%x] (%x before), bit n is code 0x500 + n",
        zox_tst_grg_errors, zox_tst_grg_errors_before);
}
