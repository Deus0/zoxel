// tst: ik_poses, the skeleton's ik rig posing a character (bones/ik): reaching, wrists, leaning, crouching
//  - bash bsh/ik_poses.sh, then look at tst/ik_poses.png (a row a pose, 4 views: +z, +x, -z, above)
//  - the body_views setup (a character of the player's kind in the void, 4 viewport cameras); its animations left on,
//      the rig's inputs set a pose: the arms' IKTarget (held, IKHeld), IKTargetRotation, the skeleton's SpineLean and
//      HipsShift, its feet planted (PlantFeet); a screenshot a pose: tst/ik_poses/<n>_<pose>.ppm
//  - checks each pose: a held hand on its target; the feet where they stood at rest (planted, on the ground) leaning
//      and crouching; crouched the hips lower and the knees bent ahead (not back, nor out); leaning the neck went the
//      lean's way
//  - right is -x, ahead +z (the body's space)
#define zox_tst_ik_poses_settle 16      // frames a pose is held before it's measured and its screenshot

typedef struct {
    const char* name;
    byte reach[2];          // a hand held (left, right)
    float3 reach_way[2];    // its target from its shoulder, of the arm's reach, in the body's space
    float roll[2];          // its turn about its forward (a wrist), radians
    float3 lean;            // the spine's lean, euler radians (x ahead, z to the side)
    float crouch;           // the hips' drop, of the leg's length
    float back;             // the hips back, of the leg's length (bending)
} zox_tst_ik_pose;

const zox_tst_ik_pose zox_tst_ik_poses[] = {
    { "rest" },
    { "reach_up", { 1, 1 }, { { 0, 0.8f, 0 }, { 0, 0.8f, 0 } } },
    { "reach_out", { 1, 1 }, { { 0.8f, 0, 0 }, { -0.8f, 0, 0 } } },
    { "reach_ahead", { 1, 1 }, { { 0, 0, 0.8f }, { 0, 0, 0.8f } } },
    { "reach_down", { 1, 1 }, { { 0.1f, -0.75f, 0.2f }, { -0.1f, -0.75f, 0.2f } } },
    { "wrists", { 1, 1 }, { { 0.2f, 0, 0.7f }, { -0.2f, 0, 0.7f } }, { 1.2f, -1.2f } },
    { "lean_ahead", .lean = { 0.5f, 0, 0 } },
    { "lean_side", .lean = { 0, 0, 0.35f } },
    { "crouch", .crouch = 0.35f },
    { "crouch_lean", { 1, 1 }, { { 0.2f, -0.2f, 0.7f }, { -0.2f, -0.2f, 0.7f } }, .lean = { 0.45f, 0, 0 }, .crouch = 0.3f },
};
#define zox_tst_ik_poses_count ((int) (sizeof(zox_tst_ik_poses) / sizeof(zox_tst_ik_poses[0])))

int zox_tst_ik_poses_stage = 0;
int zox_tst_ik_poses_index = 0;
int zox_tst_ik_poses_frames = 0;
char zox_tst_ik_poses_files[zox_tst_ik_poses_count][512];
float3 zox_tst_ik_poses_feet[2];        // the feet at rest, in the world
float3 zox_tst_ik_poses_neck;           // the neck at rest, in the body's space
float3 zox_tst_ik_poses_hips;           // the hips at rest, in the body's space
// a pose's measures
float zox_tst_ik_poses_miss[zox_tst_ik_poses_count];       // a held hand off its target, worst
float zox_tst_ik_poses_feet_moved[zox_tst_ik_poses_count]; // the feet from where they stood at rest, worst
float zox_tst_ik_poses_knee_ahead[zox_tst_ik_poses_count]; // the knees ahead of their thigh to foot line, least
float zox_tst_ik_poses_knee_out[zox_tst_ik_poses_count];   // the knees out to the side of it, most
float zox_tst_ik_poses_hips_drop[zox_tst_ik_poses_count];  // the hips lower than at rest
float3 zox_tst_ik_poses_neck_moved[zox_tst_ik_poses_count];    // the neck from rest, in the body's space

static byte zox_tst_ik_poses_legs(ecs* world, entity skeleton, ik_leg* legs) {
    const BoneLinks* bones = zox_get(skeleton, BoneLinks);
    byte count = 0;
    for (int i = 0; bones && i < bones->length && count < 2; i++) {
        if (zox_valid(bones->value[i]) && zox_has(bones->value[i], ThighBone) && ik_leg_find(world, bones->value[i], &legs[count])) {
            count++;
        }
    }
    return count;
}

// the shoulders, left (+x) then right (-x)
static byte zox_tst_ik_poses_shoulders(ecs* world, entity skeleton, entity* shoulders) {
    const BoneLinks* bones = zox_get(skeleton, BoneLinks);
    shoulders[0] = shoulders[1] = 0;
    for (int i = 0; bones && i < bones->length; i++) {
        const entity bone = bones->value[i];
        if (zox_valid(bone) && zox_has(bone, ShoulderBone) && zox_has(bone, IKWeight)) {
            shoulders[zox_getv(bone, LocalPosition3D).x < 0 ? 1 : 0] = bone;
        }
    }
    return zox_valid(shoulders[0]) && zox_valid(shoulders[1]);
}

static inline float3 zox_tst_ik_poses_in_body(ecs* world, entity character, float3 p) {
    return float4_rotate_float3(quaternion_inverse(zox_getv(character, Rotation3D)), float3_sub(p, zox_getv(character, Position3D)));
}

// the rig's inputs for a pose, every frame
static void zox_tst_ik_poses_set(ecs* world, entity character, entity skeleton, const zox_tst_ik_pose* pose) {
    const float4 facing = zox_getv(character, Rotation3D);
    entity shoulders[2];
    if (zox_tst_ik_poses_shoulders(world, skeleton, shoulders)) {
        for (int i = 0; i < 2; i++) {
            const entity shoulder = shoulders[i];
            if (!zox_has(shoulder, IKHeld)) {
                zox_add(shoulder, IKHeld);
            }
            ik_arm arm;
            if (!pose->reach[i] || !ik_arm_find(world, shoulder, &arm)) {
                zox_getm(shoulder, IKWeight)->value = 0;
                continue;
            }
            const float3 elbow = zox_getv(arm.bones[arm.length - 2], Position3D);
            const float reach = float3_distance(zox_getv(shoulder, Position3D), elbow) +
                float3_distance(elbow, zox_getv(arm.bones[arm.length - 1], Position3D));
            zox_setv(shoulder, IKTarget, float3_add(zox_getv(shoulder, Position3D),
                float4_rotate_float3(facing, float3_scale(pose->reach_way[i], reach))));
            // the hand's forward (-z) along the reach, rolled about it
            const float4 along = quaternion_from_to(float3_backward, float3_normalize(pose->reach_way[i]));
            zox_setv(shoulder, IKTargetRotation, quaternion_rotate(quaternion_rotate(facing, along),
                quaternion_from_axis_angle(float3_backward, pose->roll[i])));
            zox_setv(shoulder, IKWeight, 1);
        }
    }
    ik_leg legs[2];
    const float leg = zox_tst_ik_poses_legs(world, skeleton, legs) ? ik_leg_length(world, &legs[0]) : 0.4f;
    zox_setv(skeleton, SpineLean, quaternion_from_euler(pose->lean));
    zox_setv(skeleton, HipsShift, ((float3) { 0, -pose->crouch * leg, -pose->back * leg }));
}

static void zox_tst_ik_poses_measure(ecs* world, entity character, entity skeleton, int index) {
    const zox_tst_ik_pose* pose = &zox_tst_ik_poses[index];
    float miss = 0;
    entity shoulders[2];
    if (zox_tst_ik_poses_shoulders(world, skeleton, shoulders)) {
        for (int i = 0; i < 2; i++) {
            ik_arm arm;
            if (pose->reach[i] && ik_arm_find(world, shoulders[i], &arm)) {
                miss = float_max(miss, float3_distance(zox_getv(arm.bones[arm.length - 1], Position3D), zox_getv(shoulders[i], IKTarget)));
            }
        }
    }
    zox_tst_ik_poses_miss[index] = miss;
    ik_leg legs[2];
    float moved = 0, ahead = 1, out = 0;
    if (zox_tst_ik_poses_legs(world, skeleton, legs) == 2) {
        for (int i = 0; i < 2; i++) {
            const float3 foot = zox_getv(legs[i].foot, Position3D);
            moved = float_max(moved, float3_distance(foot, zox_tst_ik_poses_feet[i]));
            const float3 t = zox_tst_ik_poses_in_body(world, character, zox_getv(legs[i].thigh, Position3D));
            const float3 k = zox_tst_ik_poses_in_body(world, character, zox_getv(legs[i].calf, Position3D));
            const float3 f = zox_tst_ik_poses_in_body(world, character, foot);
            const float3 middle = float3_scale(float3_add(t, f), 0.5f);
            ahead = float_min(ahead, k.z - middle.z);
            out = float_max(out, fabsf(k.x - middle.x));
        }
    }
    zox_tst_ik_poses_feet_moved[index] = moved;
    zox_tst_ik_poses_knee_ahead[index] = ahead;
    zox_tst_ik_poses_knee_out[index] = out;
    spine_bones spine;
    if (spine_bones_find(world, skeleton, &spine)) {
        zox_tst_ik_poses_hips_drop[index] = zox_tst_ik_poses_hips.y -
            zox_tst_ik_poses_in_body(world, character, zox_getv(spine.hips, Position3D)).y;
        zox_tst_ik_poses_neck_moved[index] = float3_sub(
            zox_tst_ik_poses_in_body(world, character, zox_getv(spine.head, Position3D)), zox_tst_ik_poses_neck);
    }
}

byte zox_tst_ik_poses_ready(ecs* world) {
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
    if (!zox_tst_ik_poses_stage) {
        const entity active = zox_valid(skeleton) ? zox_get_link(world, skeleton, ActiveMesh) : 0;
        if (zox_getv(character, BodyDirty) != zox_generate_body_end || !zox_valid(active) ||
            zox_has(active, SkeletonMeshStale) || !is_skeleton_mesh_ready(world, active) ||
            zox_valid(zox_get_link(world, skeleton, PreparingMesh)))
        {
            zox_tst_waiting = "the body to settle";
            zox_tst_progress(zox_current_time);
            return 0;
        }
        zox_tst_mkdir(zox_tst_path("ik_poses"));
        zox_add(skeleton, PlantFeet);
        zox_tst_ik_poses_stage = 1;
        zox_tst_ik_poses_frames = 0;
    }
    zox_tst_progress(zox_tst_ik_poses_index * 1000 + zox_tst_ik_poses_frames);
    zox_tst_waiting = "the ik poses' screenshots";
    if (zox_tst_ik_poses_index >= zox_tst_ik_poses_count) {
        return !zox_screenshot_request[0];
    }
    const zox_tst_ik_pose* pose = &zox_tst_ik_poses[zox_tst_ik_poses_index];
    zox_tst_ik_poses_set(world, character, skeleton, pose);
    zox_tst_ik_poses_frames++;
    if (zox_tst_ik_poses_frames == zox_tst_ik_poses_settle - 1) {
        if (zox_tst_ik_poses_index == 0) {
            // at rest: where the feet stand, the neck and hips are
            ik_leg legs[2];
            if (zox_tst_ik_poses_legs(world, skeleton, legs) == 2) {
                for (int i = 0; i < 2; i++) {
                    zox_tst_ik_poses_feet[i] = zox_getv(legs[i].foot, Position3D);
                }
            }
            spine_bones spine;
            if (spine_bones_find(world, skeleton, &spine)) {
                zox_tst_ik_poses_neck = zox_tst_ik_poses_in_body(world, character, zox_getv(spine.head, Position3D));
                zox_tst_ik_poses_hips = zox_tst_ik_poses_in_body(world, character, zox_getv(spine.hips, Position3D));
            }
        }
        zox_tst_ik_poses_measure(world, character, skeleton, zox_tst_ik_poses_index);
    }
    if (zox_tst_ik_poses_frames == zox_tst_ik_poses_settle) {
        char name[128];
        snprintf(name, sizeof(name), "ik_poses/%02i_%s.ppm", zox_tst_ik_poses_index, pose->name);
        snprintf(zox_tst_ik_poses_files[zox_tst_ik_poses_index], sizeof(zox_tst_ik_poses_files[0]), "%s", zox_tst_path(name));
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_ik_poses_files[zox_tst_ik_poses_index]);
    } else if (zox_tst_ik_poses_frames > zox_tst_ik_poses_settle && !zox_screenshot_request[0]) {
        zox_tst_ik_poses_index++;
        zox_tst_ik_poses_frames = 0;
    }
    return 0;
}

void zox_tst_ik_poses_check(ecs* world) {
    (void) world;
    for (int i = 0; i < zox_tst_ik_poses_count; i++) {
        const zox_tst_ik_pose* pose = &zox_tst_ik_poses[i];
        const float3 neck = zox_tst_ik_poses_neck_moved[i];
        zox_log("[tst] ik pose [%s] hand off target [%.4f], feet moved [%.4f], knees ahead [%.3f] out [%.3f], hips down [%.3f], neck moved [%.3f %.3f %.3f]",
            pose->name, zox_tst_ik_poses_miss[i], zox_tst_ik_poses_feet_moved[i], zox_tst_ik_poses_knee_ahead[i],
            zox_tst_ik_poses_knee_out[i], zox_tst_ik_poses_hips_drop[i], neck.x, neck.y, neck.z);
        zox_tst_check(zox_tst_ik_poses_miss[i] < 0.01f, "[%s] a hand is off its target [%f]", pose->name, zox_tst_ik_poses_miss[i]);
        zox_tst_check(zox_tst_ik_poses_feet_moved[i] < 0.01f, "[%s] the feet moved off where they stood [%f]", pose->name, zox_tst_ik_poses_feet_moved[i]);
        if (pose->crouch > 0) {
            zox_tst_check(zox_tst_ik_poses_hips_drop[i] > pose->crouch * 0.05f, "[%s] the hips didn't drop [%f]", pose->name, zox_tst_ik_poses_hips_drop[i]);
            zox_tst_check(zox_tst_ik_poses_knee_ahead[i] > 0.01f, "[%s] the knees didn't bend ahead [%f]", pose->name, zox_tst_ik_poses_knee_ahead[i]);
            zox_tst_check(zox_tst_ik_poses_knee_out[i] < 0.02f, "[%s] the knees bent out to the side [%f]", pose->name, zox_tst_ik_poses_knee_out[i]);
        }
        if (pose->lean.x > 0) {
            zox_tst_check(neck.z > 0.05f, "[%s] leaning ahead the neck didn't go ahead [%f]", pose->name, neck.z);
        }
        if (pose->lean.z != 0) {
            zox_tst_check(fabsf(neck.x) > 0.03f, "[%s] leaning to the side the neck didn't go aside [%f]", pose->name, neck.x);
        }
        int2 size;
        byte* pixels = zox_tst_load_ppm(zox_tst_ik_poses_files[i], &size);
        zox_tst_check(pixels, "no screenshot for ik pose [%s]", pose->name);
        free(pixels);
    }
}

void zox_tsts_ik_poses(ecs* world) {
    zox_tst_add("ik_poses", zox_tst_ik_poses_ready, zox_tst_ik_poses_check, 240);
    zox_tst_set_setup("ik_poses", NULL, 1);
}
