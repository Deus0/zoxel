// tst: punch_views, the punch animation as gifs from 4 angles, to look at its motion (bones/ik/sys/swing.c, dat/swings.c)
//  - bash bsh/punch.sh (your settings), then look at tst/punch_views/punch.gif and punch_slow.gif, or punch.png
//      (punch_slow's frames in a grid)
//  - the body_views setup (a character of the player's kind in the void), its 4 cameras moved round its punching arm
//      (the right, -x): ahead (+z), the right side, ahead and right above, behind over its right shoulder (as a third
//      person camera); raised (as holding the punch skill), then punches recorded by the gif recorder (graphics/fun/gif.c)
//  - punch.gif: two punches at the punch skill's speed (its SwingTime, 0.5s); punch_slow.gif: one over 2s, each part
//      of it caught at the gif's frame rate (zox_tst_gif_fps, 20 by default)
//  - checks each gif has frames, and the slow punch's hand: at the hit out near the arm's full reach, pulled back before
//      it nearer than the guard; the off hand (the left) up with it, out as the right winds up, pulled back to the chin
//      as it lands (the recoil)
#define zox_tst_punch_views_guard 0.6       // seconds raised before a punch, and after it
const char* zox_tst_punch_views_names[2] = { "punch", "punch_slow" };
const float zox_tst_punch_views_speeds[2] = { 0.5f, 2.0f };
const int zox_tst_punch_views_punches[2] = { 2, 1 };
int zox_tst_punch_views_stage = 0;      // 0 settling, then each gif, then done
double zox_tst_punch_views_time = 0;
int zox_tst_punch_views_frames[2];
float zox_tst_punch_views_guard_out = 0;    // the hand from the shoulder, of the arm's reach
float zox_tst_punch_views_back_out = 1;
float zox_tst_punch_views_hit_out = 0;
float zox_tst_punch_views_off_back = 0;     // the off hand out of its reach, winding up (most)
float zox_tst_punch_views_off_hit = 1;      // at the hit (least)

// the hand from the shoulder, of the arm's reach
static float zox_tst_punch_views_arm_out(ecs* world, entity shoulder) {
    ik_arm arm;
    if (!zox_valid(shoulder) || !ik_arm_find(world, shoulder, &arm)) {
        return 0;
    }
    const float3 s = zox_getv(shoulder, Position3D);
    const float3 el = zox_getv(arm.bones[arm.length - 2], Position3D);
    const float3 h = zox_getv(arm.bones[arm.length - 1], Position3D);
    return float3_distance(s, h) / (float3_distance(s, el) + float3_distance(el, h));
}

static float zox_tst_punch_views_out(ecs* world, entity skeleton) {
    return zox_tst_punch_views_arm_out(world, zox_get_link(world, skeleton, ShoulderBoneLink));
}

static inline void zox_tst_punch_views_cameras(ecs* world, entity character) {
    // close on the upper body and the punching arm's reach, the legs at the views' bottoms
    const float3 target = float3_add(zox_tst_body_views_spot, (float3) { -0.08f, 0.15f, 0.15f });
    const float distance = float3_length(zox_getv(character, Bounds3D)) / sinf(25 * degreesToRadians) * 0.7f;
    const float3 directions[zox_tst_body_views_count] = {
        { 0, 0.1f, 1 },         // ahead
        { -1, 0.1f, 0 },        // the right side
        { -0.7f, 0.5f, 0.7f },  // ahead, right, above
        { -0.35f, 0.3f, -1 } }; // behind, over the right shoulder
    for (int i = 0; i < zox_tst_body_views_count; i++) {
        const float3 position = float3_add(target, float3_scale(float3_normalize(directions[i]), distance));
        zox_setv(zox_tst_body_views_cameras[i], Position3D, position);
        zox_setv(zox_tst_body_views_cameras[i], Rotation3D, camera_look_at(position, target));
    }
}

byte zox_tst_punch_views_ready(ecs* world) {
    if (zox_tst_body_views_stage < 3) {
        zox_tst_body_views_ready(world);
        return 0;
    }
    const entity character = zox_tst_body_views_character;
    if (!zox_valid(character)) {
        zox_tst_check(0, "the character is gone");
        return 1;
    }
    zox_setv(character, Position3D, zox_tst_body_views_spot);
    zox_setv(character, Rotation3D, quaternion_identity);
    if (zox_has(character, Velocity3D)) {
        zox_setv(character, Velocity3D, float3_zero);
    }
    const entity skeleton = get_skeleton(world, character);
    zox_getm(skeleton, RaiseShoulder)->value = 1;
    zox_tst_progress(zox_tst_punch_views_stage * 1000 + (zox_current_time - zox_tst_punch_views_time) * 10);
    if (zox_tst_punch_views_stage == 0) {
        const entity active = zox_get_link(world, skeleton, ActiveMesh);
        if (zox_getv(character, BodyDirty) != zox_generate_body_end || !zox_valid(active) ||
            zox_has(active, SkeletonMeshStale) || !is_skeleton_mesh_ready(world, active) ||
            zox_valid(zox_get_link(world, skeleton, PreparingMesh)))
        {
            zox_tst_waiting = "the body to settle";
            zox_tst_punch_views_time = 0;
            return 0;
        }
        zox_tst_punch_views_cameras(world, character);
        zox_tst_waiting = "the guard";
        if (!zox_tst_punch_views_time) {
            zox_tst_punch_views_time = zox_current_time;
        }
        if (zox_current_time - zox_tst_punch_views_time < zox_tst_punch_views_guard) {
            return 0;
        }
        zox_tst_punch_views_guard_out = zox_tst_punch_views_out(world, skeleton);
        zox_tst_punch_views_stage = 1;
        zox_tst_punch_views_time = 0;
    }
    if (zox_tst_punch_views_stage > 2) {
        return 1;
    }
    const int gif = zox_tst_punch_views_stage - 1;
    const float speed = zox_tst_punch_views_speeds[gif];
    const int punches = zox_tst_punch_views_punches[gif];
    zox_tst_waiting = zox_tst_punch_views_names[gif];
    if (!zox_tst_punch_views_time) {
        char name[128];
        snprintf(name, sizeof(name), "punch_views/%s.gif", zox_tst_punch_views_names[gif]);
        const char* env = getenv("zox_tst_gif_fps");
        zox_gif_record(zox_tst_path(name), env && env[0] ? atof(env) : 20);
        zox_tst_punch_views_time = zox_current_time;
        return 0;
    }
    // guard, the punches back to back, guard
    const double t = zox_current_time - zox_tst_punch_views_time - zox_tst_punch_views_guard;
    const int punch = t < 0 ? -1 : (int) (t / speed);
    if (punch >= 0 && punch < punches && zox_getv(skeleton, SwingStart) < zox_tst_punch_views_time + zox_tst_punch_views_guard + punch * speed) {
        zox_setv(skeleton, SwingKind, zox_swing_punch);
        zox_setv(skeleton, SwingStart, zox_current_time);
        zox_setv(skeleton, SwingSpeed, speed);
    }
    if (gif == 1 && punch == 0) {
        const float p = (float) (t / speed);
        const float out = zox_tst_punch_views_out(world, skeleton);
        const float hit = swings[zox_swing_punch].marks[0];
        if (p > 0.2f && p < swings[zox_swing_punch].back_end + 0.02f && out < zox_tst_punch_views_back_out) {
            zox_tst_punch_views_back_out = out;
        }
        if (p > hit - 0.02f && p < swings[zox_swing_punch].held_end && out > zox_tst_punch_views_hit_out) {
            zox_tst_punch_views_hit_out = out;
        }
        const float off = zox_tst_punch_views_arm_out(world, zox_get_link(world, skeleton, OffShoulderBoneLink));
        if (p > 0.2f && p < swings[zox_swing_punch].back_end + 0.02f && off > zox_tst_punch_views_off_back) {
            zox_tst_punch_views_off_back = off;
        }
        if (p > hit - 0.02f && p < swings[zox_swing_punch].held_end && off < zox_tst_punch_views_off_hit) {
            zox_tst_punch_views_off_hit = off;
        }
    }
    if (t > punches * speed + zox_tst_punch_views_guard) {
        zox_tst_punch_views_frames[gif] = zox_gif_stop();
        zox_tst_punch_views_stage++;
        zox_tst_punch_views_time = 0;
    }
    return 0;
}

void zox_tst_punch_views_check(ecs* world) {
    (void) world;
    zox_gif_stop();
    for (int i = 0; i < 2; i++) {
        zox_log("[tst] punch_views: [%s.gif] [%i] frames", zox_tst_punch_views_names[i], zox_tst_punch_views_frames[i]);
        zox_tst_check(zox_tst_punch_views_frames[i] >= 4, "[%s.gif] has [%i] frames", zox_tst_punch_views_names[i],
            zox_tst_punch_views_frames[i]);
    }
    zox_log("[tst] punch_views: the hand out of reach: guard [%.3f], pulled back [%.3f], hit [%.3f]",
        zox_tst_punch_views_guard_out, zox_tst_punch_views_back_out, zox_tst_punch_views_hit_out);
    zox_tst_check(zox_tst_punch_views_back_out < zox_tst_punch_views_guard_out - 0.05f, "pulled back, the hand wasn't nearer than the guard [%f]", zox_tst_punch_views_back_out);
    zox_tst_check(zox_tst_punch_views_hit_out > 0.9f, "at the hit the arm wasn't out [%f]", zox_tst_punch_views_hit_out);
    zox_log("[tst] punch_views: the off hand out of reach: winding up [%.3f], at the hit [%.3f]",
        zox_tst_punch_views_off_back, zox_tst_punch_views_off_hit);
    zox_tst_check(zox_tst_punch_views_off_back > 0.7f && zox_tst_punch_views_off_back < 0.95f, "winding up, the off hand wasn't up ahead, bent [%f]", zox_tst_punch_views_off_back);
    zox_tst_check(zox_tst_punch_views_off_hit < zox_tst_punch_views_off_back - 0.1f, "at the hit, the off hand wasn't pulled back [%f]", zox_tst_punch_views_off_hit);
}

void zox_tsts_punch_views(ecs* world) {
    zox_tst_add("punch_views", zox_tst_punch_views_ready, zox_tst_punch_views_check, 120);
    zox_tst_set_setup("punch_views", NULL, 1);
}
