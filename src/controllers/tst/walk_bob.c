// tst: walk_bob, walking the head (the first person camera on it) bobs once a step, smoothly: down and up
//  - the body bobs over its stance foot (arm_control.c); the lifted swinging knee counted in it made two bumps a step
//      (walking felt shaky)
//  - on the test level, a gamepad's stick ahead for a few seconds; the head's height over the body each frame, its
//      turns (down to up, up to down) against the steps (SwingPhase, a step a half cycle), and how far it bobs
//  - run: bash bsh/tst.sh walk_bob
#define zox_tst_bob_settle 1.0      // seconds walking before it's measured
#define zox_tst_bob_measure 3.0
int zox_tst_bob_stage = 0;
double zox_tst_bob_time = 0;
float zox_tst_bob_last = 0, zox_tst_bob_last_delta = 0;
float zox_tst_bob_min = 999, zox_tst_bob_max = -999;
float zox_tst_bob_phase_last = 0, zox_tst_bob_phase_total = 0;     // the phase walked (it wraps round)
int zox_tst_bob_turns = 0, zox_tst_bob_samples = 0;

byte zox_tst_walk_bob_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity skeleton = get_skeleton(world, character);
    if (!zox_valid(skeleton) || !zox_has(skeleton, SwingPhase)) {
        zox_tst_waiting = "the player's skeleton";
        return 0;
    }
    const double t = zox_current_time - zox_tst_bob_time;
    zox_tst_progress(zox_tst_bob_stage * 100 + t * 10);
    if (zox_tst_bob_stage == 0) {
        zox_tst_pad = zox_tst_spawn_pad(world, dbg_player);
        zox_tst_bob_stage = 1;
        zox_tst_bob_time = zox_current_time;
        return 0;
    }
    if (zox_tst_bob_stage == 1) {
        // the stick touched, so the gamepad's the player's device (its first input switches to it, unseen)
        zox_tst_waiting = "the gamepad's mode";
        zox_tst_pad_stick = (float2) { 0, t < 0.1 ? 0.05f : 0 };
        if (t > 0.5) {
            zox_tst_bob_stage = 2;
            zox_tst_bob_time = zox_current_time;
        }
        return 0;
    }
    zox_tst_waiting = "walking";
    zox_tst_pad_stick = (float2) { 0, 1 };
    if (t < zox_tst_bob_settle) {
        return 0;
    }
    const float head = zox_tst_crouch_head(world, character);
    const float phase = zox_getv(skeleton, SwingPhase);
    if (zox_tst_bob_samples) {
        float step = phase - zox_tst_bob_phase_last;
        while (step < -M_PI) step += 2 * M_PI;
        while (step > M_PI) step -= 2 * M_PI;
        zox_tst_bob_phase_total += fabsf(step);
        // a turn: the head's way changes (moves under 0.1mm ignored, a frame at the top or bottom)
        const float delta = head - zox_tst_bob_last;
        if (fabsf(delta) > 0.0001f) {
            if (zox_tst_bob_last_delta && (delta > 0) != (zox_tst_bob_last_delta > 0)) {
                zox_tst_bob_turns++;
            }
            zox_tst_bob_last_delta = delta;
        }
    }
    zox_tst_bob_last = head;
    zox_tst_bob_phase_last = phase;
    zox_tst_bob_min = fminf(zox_tst_bob_min, head);
    zox_tst_bob_max = fmaxf(zox_tst_bob_max, head);
    zox_tst_bob_samples++;
    if (t < zox_tst_bob_settle + zox_tst_bob_measure) {
        return 0;
    }
    zox_tst_pad_stick = float2_zero;
    return 1;
}

void zox_tst_walk_bob_check(ecs* world) {
    (void) world;
    const float steps = zox_tst_bob_phase_total / (float) M_PI;
    const float bob = zox_tst_bob_max - zox_tst_bob_min;
    zox_log("[tst] walk_bob: [%.1f] steps over [%i] frames, the head turned [%i] times, bobbed [%.1f]cm",
        steps, zox_tst_bob_samples, zox_tst_bob_turns, bob * 100);
    zox_tst_check(steps > 3, "only [%.1f] steps walked", steps);
    // down and up once a step (two turns), a turn or two spare at the ends
    zox_tst_check(zox_tst_bob_turns <= 2 * steps + 2, "the head turned [%i] times in [%.1f] steps, more than down and up once each",
        zox_tst_bob_turns, steps);
    zox_tst_check(bob > 0.005f && bob < 0.06f, "the head bobbed [%.1f]cm", bob * 100);
}

void zox_tsts_walk_bob(ecs* world) {
    zox_tst_add("walk_bob", zox_tst_walk_bob_ready, zox_tst_walk_bob_check, 60);
    zox_tst_set_setup("walk_bob", zox_tst_level_setup, 0);
}
