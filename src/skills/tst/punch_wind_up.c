// tst: punch_wind_up, the punch skill's warmup starts the arm's swing (sys/melee_swing.c), so it winds up before
//  the hit, not after it (it began at the hit: the pull back after contact looked like a pat)
//  - the player's arm raised (as holding the punch), its punch skill slowed and begun (ActivateBegin, as its key does)
//  - in the warmup the skeleton's swing has started, at the warmup's start, over warmup + cooldown
//  - before the hit the hand is pulled back in (of the arm's reach, the guard's out 0.85), at the hit it's out
//  - the body turns away winding up, into the strike (SpineTwist): the shoulder goes forward, the first person
//    camera (on the head, turned back by it) holds still
//  - run: bash bsh/tst.sh punch_wind_up
#define zox_tst_wind_seconds 1.5   // the warmup and cooldown slowed, so slow frames still land in the wind up
byte zox_tst_wind_stage = 0;
double zox_tst_wind_at = 0;
entity zox_tst_wind_skill = 0;
byte zox_tst_wind_swung = 0;        // the swing started in the warmup
double zox_tst_wind_swing_start = 0;
float zox_tst_wind_swing_speed = 0;
float zox_tst_wind_pulled = 1;      // the hand's least reach in the warmup
float zox_tst_wind_hit_out = 0;     // its most just after the hit
int zox_tst_wind_samples = 0;       // frames in the warmup
float zox_tst_wind_shoulder_back = 1000;    // the shoulder's least forward (the skeleton's +z) wound up
float zox_tst_wind_shoulder_out = -1000;    // its most at the strike, turned into it
float zox_tst_wind_shoulder_side = 0;       // its side (x), the twist's lever
float4 zox_tst_wind_camera_from;
float zox_tst_wind_camera_turn = 0;         // the camera's most turn (radians) through the punch
byte zox_tst_wind_has_camera = 0;

static float zox_tst_wind_out(ecs* world, entity shoulder) {
    ik_arm arm;
    if (!ik_arm_find(world, shoulder, &arm)) {
        return 0;
    }
    const float3 s = zox_getv(shoulder, Position3D);
    const float3 el = zox_getv(arm.bones[arm.length - 2], Position3D);
    const float3 h = zox_getv(arm.bones[arm.length - 1], Position3D);
    return float3_distance(s, h) / (float3_distance(s, el) + float3_distance(el, h));
}

// the shoulder in the skeleton's space (the body turned: its twist moves it)
static float3 zox_tst_wind_shoulder(ecs* world, entity skeleton, entity shoulder) {
    return float4_rotate_float3(quaternion_inverse(zox_getv(skeleton, Rotation3D)),
        float3_sub(zox_getv(shoulder, Position3D), zox_getv(skeleton, Position3D)));
}

static entity zox_tst_wind_camera(ecs* world, entity skeleton) {
    const entity head = zox_get_link(world, skeleton, HeadBoneLink);
    return zox_valid(head) ? zox_get_child_by_id(world, head, zox_id(Camera)) : 0;
}

static entity zox_tst_wind_find_melee(ecs* world, entity user) {
    entity children[zox_children_capacity];
    const uint count = zox_get_children(world, user, children, zox_children_capacity);
    for (uint i = 0; i < count; i++) {
        if (zox_has(children[i], Melee)) {
            return children[i];
        }
    }
    return 0;
}

byte zox_tst_punch_wind_up_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity skeleton = zox_get_link(world, character, SkeletonLink);
    const entity shoulder = zox_valid(skeleton) ? zox_get_link(world, skeleton, ShoulderBoneLink) : 0;
    if (!zox_valid(shoulder)) {
        zox_tst_waiting = "the player's right arm";
        return 0;
    }
    if (!zox_tst_wind_skill) {
        zox_tst_wind_skill = zox_tst_wind_find_melee(world, character);
        zox_tst_waiting = "the player's punch skill";
        return 0;
    }
    if (!zox_tst_wind_at) {
        zox_tst_wind_at = zox_current_time;
    }
    zox_getm(skeleton, RaiseShoulder)->value = 1;
    const entity skill = zox_tst_wind_skill;
    const double t = zox_current_time - zox_tst_wind_at;
    zox_tst_progress(zox_tst_wind_stage * 1000 + t * 10);
    if (zox_tst_wind_stage == 0) {
        zox_tst_waiting = "the guard";
        if (t > 1.5) {
            zox_setv(skill, WarmupTime, zox_tst_wind_seconds);
            zox_setv(skill, CooldownTime, zox_tst_wind_seconds);
            zox_setv(skill, ActivateBegin, zox_dirty_trigger);
            zox_tst_wind_stage = 1;
        }
        return 0;
    }
    const double warmup_at = zox_getv(skill, WarmupAt);
    if (zox_tst_wind_stage == 1) {
        zox_tst_waiting = "the warmup";
        if (warmup_at) {
            zox_tst_wind_stage = 2;
            const entity camera = zox_tst_wind_camera(world, skeleton);
            zox_tst_wind_has_camera = zox_valid(camera);
            if (zox_tst_wind_has_camera) {
                zox_tst_wind_camera_from = zox_getv(camera, Rotation3D);
            }
            zox_tst_wind_shoulder_side = fabsf(zox_tst_wind_shoulder(world, skeleton, shoulder).x);
        } else if (t > 5) {
            return 1;   // never warmed up, the checks say so
        }
        return 0;
    }
    const float out = zox_tst_wind_out(world, shoulder);
    const float forward = zox_tst_wind_shoulder(world, skeleton, shoulder).z;
    const entity camera = zox_tst_wind_camera(world, skeleton);
    if (zox_tst_wind_has_camera && zox_valid(camera)) {
        const float d = fabsf(float4_dot(zox_tst_wind_camera_from, zox_getv(camera, Rotation3D)));
        const float turn = 2 * acosf(d > 1 ? 1 : d);
        if (turn > zox_tst_wind_camera_turn) {
            zox_tst_wind_camera_turn = turn;
        }
    }
    if (zox_tst_wind_stage == 2) {
        zox_tst_waiting = "the warmup's end";
        if (warmup_at) {
            if (zox_getv(skeleton, SwingStart) == warmup_at) {
                zox_tst_wind_swung = 1;
                zox_tst_wind_swing_start = warmup_at;
                zox_tst_wind_swing_speed = zox_getv(skeleton, SwingSpeed);
            }
            zox_tst_wind_samples++;
            if (forward < zox_tst_wind_shoulder_back) {
                zox_tst_wind_shoulder_back = forward;
            }
            if (out < zox_tst_wind_pulled) {
                zox_tst_wind_pulled = out;
            }
            return 0;
        }
        zox_tst_wind_stage = 3;
        zox_tst_wind_at = zox_current_time;
        return 0;
    }
    // just after the hit: the strike holds out a moment
    if (out > zox_tst_wind_hit_out) {
        zox_tst_wind_hit_out = out;
    }
    if (forward > zox_tst_wind_shoulder_out) {
        zox_tst_wind_shoulder_out = forward;
    }
    zox_tst_waiting = "the strike";
    return zox_current_time - zox_tst_wind_at > 0.3;
}

void zox_tst_punch_wind_up_check(ecs* world) {
    const entity skill = zox_tst_wind_skill;
    const float expected = zox_valid(skill) ? zox_getv(skill, WarmupTime) + zox_getv(skill, CooldownTime) : 0;
    zox_log("[tst] punch_wind_up: swung in the warmup [%i] over [%.3f]s of [%.3f], pulled in [%.3f] over [%i] frames, hit out [%.3f]",
        zox_tst_wind_swung, zox_tst_wind_swing_speed, expected, zox_tst_wind_pulled, zox_tst_wind_samples, zox_tst_wind_hit_out);
    zox_tst_check(zox_valid(skill), "the player has no punch skill");
    zox_tst_check(zox_tst_wind_stage >= 2, "the punch never warmed up");
    zox_tst_check(zox_tst_wind_swung, "the swing didn't start with the warmup");
    zox_tst_check(fabsf(zox_tst_wind_swing_speed - expected) < 0.001f, "the swing isn't over warmup and cooldown [%f of %f]", zox_tst_wind_swing_speed, expected);
    zox_tst_check(zox_tst_wind_pulled < 0.75f, "the hand didn't wind back before the hit [%f]", zox_tst_wind_pulled);
    zox_tst_check(zox_tst_wind_hit_out > 0.9f, "the hand wasn't out at the hit [%f]", zox_tst_wind_hit_out);
    // turned away then into it: the twists (-0.35 to 0.3 radians) move the shoulder ~0.6 of its side forward
    const float turned = zox_tst_wind_shoulder_out - zox_tst_wind_shoulder_back;
    zox_log("[tst] punch_wind_up: the shoulder went forward [%.3f] (its side [%.3f]), the camera turned [%.3f]",
        turned, zox_tst_wind_shoulder_side, zox_tst_wind_camera_turn);
    zox_tst_check(turned > 0.3f * zox_tst_wind_shoulder_side, "the body didn't turn into the punch [%f of %f]", turned, zox_tst_wind_shoulder_side);
    if (zox_tst_wind_has_camera) {
        zox_tst_check(zox_tst_wind_camera_turn < 0.03f, "the first person camera turned with the body [%f]", zox_tst_wind_camera_turn);
    }
}

void zox_tsts_punch_wind_up(ecs* world) {
    zox_tst_add("punch_wind_up", zox_tst_punch_wind_up_ready, zox_tst_punch_wind_up_check, 120);
}
