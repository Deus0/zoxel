// tst: physics_turn, first person mouse turning, at once (player_physics_turn 0, the old turn) and by physics (1,
//  setting "Physics Turn": the mouse turns FaceDirection, the move's torque turns the body after, move.c)
//  - the fake mouse moved zox_tst_pt_step pixels a frame for zox_tst_pt_frames frames, each way
//  - at once: turned exactly the mouse's turn, from its first frame; by physics: behind it at first, then settled on
//    it, its spin stopped
//  - run: bash bsh/tst.sh physics_turn
#define zox_tst_pt_step 20
#define zox_tst_pt_frames 10
byte zox_tst_pt_stage = 0;
int zox_tst_pt_frame = 0;
double zox_tst_pt_time = 0;
float zox_tst_pt_start = 0;
float zox_tst_pt_instant = 0, zox_tst_pt_lagged = 0, zox_tst_pt_settled = 0, zox_tst_pt_spin = 0;
float zox_tst_pt_first_instant = 0, zox_tst_pt_first_lagged = 0;   // one frame into the turn

static inline float zox_tst_pt_yaw(ecs* world, entity character) {
    return quaternion_to_euler_y2(zox_getv(character, Rotation3D));
}

static inline float zox_tst_pt_turned(ecs* world, entity character) {
    return fabsf(remainderf(zox_tst_pt_yaw(world, character) - zox_tst_pt_start, 2 * M_PI));
}

byte zox_tst_physics_turn_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    if (zox_has(character, DisableMovement) || zox_getv(dbg_player, PlayerState) != zox_player_state_playing) {
        zox_tst_waiting = "the player playing";
        return 0;
    }
    zox_tst_progress(zox_tst_pt_stage * 100 + zox_tst_pt_frame);
    zox_mouse_fake = 1;
    // 0, 2: start a turn (at once, by physics); 1, 3: the mouse moving
    if (zox_tst_pt_stage == 0 || zox_tst_pt_stage == 2) {
        zox_tst_waiting = "a still body";
        player_physics_turn = zox_tst_pt_stage == 2;
        // the mouse in use, its device enabled (device_switch_system does, on a mouse's input)
        const entity mouse = zox_get_child_by_id(world, dbg_player, zox_id(Mouse));
        if (zox_valid(mouse) && zox_has(mouse, Disabled)) {
            zox_setv(dbg_player, DeviceMode, zox_device_mode_keyboardmouse);
            zox_set_tag(mouse, Disabled, 0);
        }
        if (zox_current_time - zox_tst_pt_time < 1) {
            return 0;
        }
        zox_tst_pt_start = zox_tst_pt_yaw(world, character);
        zox_tst_pt_frame = 0;
        zox_tst_pt_stage++;
        return 0;
    }
    if (zox_tst_pt_stage == 1 || zox_tst_pt_stage == 3) {
        zox_tst_waiting = "the mouse turning";
        // the first move read: a frame after it (extracted in zoxp_spawn, turned in zoxp_update)
        if (zox_tst_pt_frame == 1) {
            *(zox_tst_pt_stage == 1 ? &zox_tst_pt_first_instant : &zox_tst_pt_first_lagged) = zox_tst_pt_turned(world, character);
        }
        if (zox_tst_pt_frame < zox_tst_pt_frames) {
            zox_mouse_fake_position.x += zox_tst_pt_step;
            zox_tst_pt_frame++;
            return 0;
        }
        if (zox_tst_pt_frame == zox_tst_pt_frames) {
            zox_tst_pt_frame++;
            return 0;
        }
        if (zox_tst_pt_stage == 1) {
            zox_tst_pt_instant = zox_tst_pt_turned(world, character);
        } else {
            zox_tst_pt_lagged = zox_tst_pt_turned(world, character);
        }
        zox_tst_pt_time = zox_current_time;
        zox_tst_pt_stage++;
        return 0;
    }
    zox_tst_waiting = "the body to settle";
    if (zox_current_time - zox_tst_pt_time < 1.5) {
        return 0;
    }
    zox_tst_pt_settled = zox_tst_pt_turned(world, character);
    zox_tst_pt_spin = fabsf(zox_getv(character, Omega3D).y);
    return 1;
}

void zox_tst_physics_turn_check(ecs* world) {
    (void) world;
    zox_mouse_fake = 0;
    player_physics_turn = 0;
    const float wanted = zox_tst_pt_step * zox_tst_pt_frames * 0.0032f;    // rotation.c's mouse_rotate_multiplier
    const float first = zox_tst_pt_step * 0.0032f;
    zox_log("[tst] physics_turn: wanted [%.3f], at once [%.3f] (first frame [%.4f] of [%.4f]); by physics as the mouse stopped [%.3f] (first frame [%.4f]), settled [%.3f], spin [%.4f]",
        wanted, zox_tst_pt_instant, zox_tst_pt_first_instant, first, zox_tst_pt_lagged, zox_tst_pt_first_lagged, zox_tst_pt_settled, zox_tst_pt_spin);
    zox_tst_check(zox_tst_pt_stage == 4, "stopped at stage [%i]", zox_tst_pt_stage);
    zox_tst_check(fabsf(zox_tst_pt_instant - wanted) < 0.01f, "at once it turned [%.3f] of [%.3f]", zox_tst_pt_instant, wanted);
    zox_tst_check(fabsf(zox_tst_pt_first_instant - first) < 0.002f, "at once its first frame turned [%.4f] of [%.4f]", zox_tst_pt_first_instant, first);
    zox_tst_check(zox_tst_pt_first_lagged < first * 0.8f, "by physics its first frame turned at once [%.4f] of [%.4f]", zox_tst_pt_first_lagged, first);
    zox_tst_check(zox_tst_pt_lagged > wanted * 0.5f, "by physics it fell behind the mouse [%.3f] of [%.3f]", zox_tst_pt_lagged, wanted);
    zox_tst_check(fabsf(zox_tst_pt_settled - wanted) < 0.02f, "by physics it settled at [%.3f] of [%.3f]", zox_tst_pt_settled, wanted);
    zox_tst_check(zox_tst_pt_spin < 0.05f, "by physics it still spins [%.4f]", zox_tst_pt_spin);
}

void zox_tsts_physics_turn(ecs* world) {
    (void) world;
    zox_tst_add("physics_turn", zox_tst_physics_turn_ready, zox_tst_physics_turn_check, 90);
    zox_tst_set_setup("physics_turn", zox_tst_level_setup, 0);
}
