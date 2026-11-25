// tst: gpu_restore_game, a gpu dispose and restore in a game (chunks, lod meshes, characters' skeletons, particles)
//  - live gpu objects (zox_tst_gr_live): none after a dispose, as many as before after a restore in the same frame
//  - while disposed (android's lost context, no gpu to make things on) nothing makes a gpu object
//  - restored, no new opengl errors over the next frames (ones from before are left to their own bug)
//  - run: bash bsh/tst.sh gpu_restore_game
//  - frames, not seconds: a system that makes gpu objects (dirty meshes, textures) does so within a frame or two
//  - drawn again: as many meshes draw (MeshRenderCount) after as before, within 600 frames (they mesh again over
//    frames, gpu meshes from their grids), and the picture's average colour back near the one before (gpu_restore's
//    compare: the sky where the terrain was changes it); the terrain stayed gone after a vulkan restore on android
//    (2026-11-21), its gpu meshes kept their old counts
//  - game time stopped from the first screenshot to the second (updating_time, a restore starts it again): a hit on
//    the player between them (its particles) changed the picture more than the missing terrain did
#define zox_tst_grg_stage_frames 10
int zox_tst_grg_stage = 0, zox_tst_grg_frames = 0;
int zox_tst_grg_before[zox_tst_gr_kinds], zox_tst_grg_disposed[zox_tst_gr_kinds], zox_tst_grg_restored[zox_tst_gr_kinds];
int zox_tst_grg_made[zox_tst_gr_kinds];     // the most made while disposed, over its frames
int zox_tst_grg_errors = 0, zox_tst_grg_errors_before = 0;    // opengl errors seen (zox_gpu_error_bits)
int zox_tst_grg_drawing_before = 0, zox_tst_grg_drawing_after = 0;
byte zox_tst_grg_shot = 0;      // a screenshot asked for (before, then after)
float zox_tst_grg_changed = 0;  // the average colour's change (0 to 765)

// meshes with something to draw
int zox_tst_grg_drawing(ecs* world) {
    int count = 0;
    zox_each_begin(MeshRenderCount);
        zox_each_o(MeshRenderCount, render_count);
        if (render_count->value) count++;
    zox_each_end();
    return count;
}

// things alive for a while (DestroyInTime running: particles, effects), which come and go between the two shots
int zox_tst_grg_particles(ecs* world) {
    int count = 0;
    zox_each_begin(DestroyInTime);
        zox_each_o(DestroyInTime, life);
        count += life->value > 0;
    zox_each_end();
    return count;
}

byte zox_tst_gpu_restore_game_ready(ecs* world) {
    zox_tst_progress(zox_tst_grg_stage * 1000 + zox_tst_grg_frames);
    if (zox_tst_grg_stage == 0) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        // a few frames in game first, opengl errors from before the restore are another bug's
        zox_tst_waiting = "the game to settle";
        zox_tst_grg_errors_before |= zox_gpu_error_bits();
        if (++zox_tst_grg_frames < zox_tst_grg_stage_frames) {
            return 0;
        }
        // its effects gone first (a burst of particles in shot a, gone by b, was the picture changing), at most 10s
        zox_tst_waiting = "the game to settle, no effects (DestroyInTime) alive";
        if (!zox_tst_grg_shot && zox_tst_grg_particles(world) && zox_tst_grg_frames < zox_tst_grg_stage_frames + 600) {
            return 0;
        }
        if (!zox_tst_grg_shot) {
            updating_time = 0;
            zox_tst_gr_screenshot("gpu_restore_game_a.ppm");
            zox_tst_grg_shot = 1;
        }
        if (zox_screenshot_request[0]) {
            zox_tst_waiting = "the screenshot before";
            return 0;
        }
        zox_tst_grg_shot = 0;
        // in one frame: nothing else makes or lets go of anything between
        zox_tst_gr_live(zox_tst_grg_before);
        zox_tst_grg_drawing_before = zox_tst_grg_drawing(world);
        opengl_dispose_resources(world);
        zox_tst_gr_live(zox_tst_grg_disposed);
        opengl_restore_resources(world);
        updating_time = 0;
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
        if (zox_tst_grg_frames < zox_tst_grg_stage_frames) {
            return 0;
        }
        opengl_restore_resources(world);
        updating_time = 0;
        zox_tst_grg_stage = 2;
        zox_tst_grg_frames = 0;
        return 0;
    }
    zox_tst_waiting = "frames restored";
    zox_tst_grg_errors |= zox_gpu_error_bits();
    if (zox_tst_grg_stage == 3 && zox_tst_grg_frames >= zox_tst_grg_stage_frames) {
        // then disposed for a while
        opengl_dispose_resources(world);
        zox_tst_grg_stage = 1;
        zox_tst_grg_frames = 0;
        return 0;
    }
    if (zox_tst_grg_stage != 2 || zox_tst_grg_frames < zox_tst_grg_stage_frames) {
        return 0;
    }
    zox_tst_waiting = "meshes drawn again";
    if (!zox_tst_grg_shot) {
        zox_tst_grg_drawing_after = zox_tst_grg_drawing(world);
        if (zox_tst_grg_drawing_after < zox_tst_grg_drawing_before && zox_tst_grg_frames < 600) {
            return 0;
        }
        zox_tst_gr_screenshot("gpu_restore_game_b.ppm");
        zox_tst_grg_shot = 1;
    }
    if (zox_screenshot_request[0]) {
        zox_tst_waiting = "the screenshot after";
        return 0;
    }
    zox_tst_grg_changed = zox_tst_gr_compare("gpu_restore_game_a.ppm", "gpu_restore_game_b.ppm");
    updating_time = 1;
    return 1;
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
    zox_log("[tst] [metric] meshes drawing: [%i] before, [%i] restored", zox_tst_grg_drawing_before, zox_tst_grg_drawing_after);
    zox_tst_check(zox_tst_grg_drawing_after >= zox_tst_grg_drawing_before * 9 / 10, "restored, [%i] meshes drawing, [%i] before",
        zox_tst_grg_drawing_after, zox_tst_grg_drawing_before);
    zox_log("[tst] [metric] the picture's average colour changed [%.1f] by the restore", zox_tst_grg_changed);
    zox_tst_check(zox_tst_grg_changed <= 5, "restored, the picture's average colour changed [%.1f]", zox_tst_grg_changed);
    // ones it had before are another bug (doc/todo.md), not the restore's
    zox_tst_check(!(zox_tst_grg_errors & ~zox_tst_grg_errors_before), "opengl errors after restoring, bits [%x] (%x before), bit n is code 0x500 + n",
        zox_tst_grg_errors, zox_tst_grg_errors_before);
}
