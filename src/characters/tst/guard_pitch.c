// tst: guard_pitch, the raised guard looking up and down (bones/ik/sys/swing.c, ik/sys/arm_ik.c)
//  - raised, the first person camera pitched level, up 60, up 85 and down 60 degrees, a second each
//  - looking level and up the hand is in the camera's view; at every pitch it's ahead of the head, not over and behind
//    it, and the elbow is bent (it locked straight looking up, the shoulder turned half round)
//  - from up 60 to up 85 the upper arm turns a little, not half a turn about itself (the shortest turn flipped it)
//  - run: bash bsh/tst.sh guard_pitch
#define zox_tst_gp_stages 4
const float zox_tst_gp_pitches[zox_tst_gp_stages] = { 0, 60, 85, -60 };   // degrees, up positive
double zox_tst_gp_start = 0;
float3 zox_tst_gp_seen[zox_tst_gp_stages];     // the hand in the camera's space (it faces -z)
float zox_tst_gp_ahead[zox_tst_gp_stages];     // the hand from the head along the body's level facing, of the reach
float zox_tst_gp_bend[zox_tst_gp_stages];      // the elbow's w: 1 straight
float4 zox_tst_gp_upper[zox_tst_gp_stages];    // the shoulder's world turn
byte zox_tst_gp_measured = 0;

byte zox_tst_guard_pitch_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_tst_player_character(world);
    const entity skeleton = zox_get_link(world, character, SkeletonLink);
    const entity shoulder = zox_valid(skeleton) ? zox_get_link(world, skeleton, ShoulderBoneLink) : 0;
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    const entity camera = zox_valid(head) ? zox_get_child_by_id(world, head, zox_id(Camera)) : 0;
    ik_arm arm;
    if (!zox_valid(camera) || !zox_valid(shoulder) || !ik_arm_find(world, shoulder, &arm)) {
        zox_tst_waiting = "the player's arm and first person camera";
        return 0;
    }
    if (!zox_tst_gp_start) {
        zox_tst_gp_start = zox_current_time;
    }
    const double t = zox_current_time - zox_tst_gp_start;
    const int stage = (int) (t / 1.2);
    zox_tst_progress(t * 10);
    zox_getm(skeleton, RaiseShoulder)->value = 1;
    if (stage >= zox_tst_gp_stages) {
        return 1;
    }
    // the camera's pitch as a mouse sets it (controllers3/sys/rotation.c), its sign the one that looks up
    float3 euler = quaternion_to_euler(zox_getv(camera, LocalRotation3D));
    const float pitch = zox_tst_gp_pitches[stage] * degrees_to_radians;
    const float3 probe = { 0.5f, euler.y, euler.z };
    const float3 probe_ahead = float4_rotate_float3(euler_to_quaternion(probe), (float3) { 0, 0, -1 });
    euler.x = probe_ahead.y > 0 ? pitch : -pitch;
    zox_setv(camera, LocalRotation3D, euler_to_quaternion(euler));
    zox_tst_waiting = "the guard at each pitch";
    if (t - stage * 1.2 < 1.0) {
        return 0;
    }
    const entity elbow = arm.bones[arm.length - 2];
    const float3 s = zox_getv(shoulder, Position3D);
    const float3 el = zox_getv(elbow, Position3D);
    const float3 hand = zox_getv(arm.bones[arm.length - 1], Position3D);
    const float reach = float3_distance(s, el) + float3_distance(el, hand);
    const float3 facing = float4_rotate_float3(zox_getv(character, Rotation3D), float3_forward);
    const float3 level = float3_normalize((float3) { facing.x, 0, facing.z });
    zox_tst_gp_seen[stage] = float4_rotate_float3(quaternion_inverse(zox_getv(camera, Rotation3D)),
        float3_sub(hand, zox_getv(camera, Position3D)));
    zox_tst_gp_ahead[stage] = float3_dot(float3_sub(hand, zox_getv(head, Position3D)), level) / reach;
    zox_tst_gp_bend[stage] = fabsf(zox_getv(elbow, LocalRotation3D).w);
    zox_tst_gp_upper[stage] = zox_getv(shoulder, Rotation3D);
    zox_tst_gp_measured |= 1 << stage;
    return 0;
}

void zox_tst_guard_pitch_check(ecs* world) {
    (void) world;
    for (int i = 0; i < zox_tst_gp_stages; i++) {
        const float3 v = zox_tst_gp_seen[i];
        zox_log("[tst] guard_pitch [%.0f]: hand in view [%.3f %.3f %.3f], ahead of the head [%.2f] of reach, elbow w [%.3f]",
            zox_tst_gp_pitches[i], v.x, v.y, v.z, zox_tst_gp_ahead[i], zox_tst_gp_bend[i]);
        if (!(zox_tst_gp_measured & (1 << i))) {
            zox_tst_check(0, "the guard at [%.0f] degrees was never measured", zox_tst_gp_pitches[i]);
            continue;
        }
        if (zox_tst_gp_pitches[i] >= 0) {
            // ahead (-z), on a 4:3 screen (as punch_motion)
            zox_tst_check(v.z < 0 && fabsf(v.x) < -v.z * 1.33f && fabsf(v.y) < -v.z,
                "looking [%.0f] degrees up the hand is out of view [%.3f %.3f %.3f]", zox_tst_gp_pitches[i], v.x, v.y, v.z);
        }
        zox_tst_check(zox_tst_gp_ahead[i] > 0, "looking [%.0f] degrees up the hand is behind the head [%.2f]",
            zox_tst_gp_pitches[i], zox_tst_gp_ahead[i]);
        zox_tst_check(zox_tst_gp_bend[i] < 0.995f, "looking [%.0f] degrees up the elbow is straight [%.3f]",
            zox_tst_gp_pitches[i], zox_tst_gp_bend[i]);
    }
    const float4 a = zox_tst_gp_upper[1], b = zox_tst_gp_upper[2];
    const float turned = 2 * acosf(float_min(1, fabsf(a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w))) * radians_to_degrees;
    zox_log("[tst] guard_pitch: the upper arm turned [%.1f] degrees from up 60 to up 85", turned);
    zox_tst_check(turned < 30, "from up 60 to up 85 the upper arm turned [%.1f] degrees, flipped about itself", turned);
}

void zox_tsts_guard_pitch(ecs* world) {
    zox_tst_add("guard_pitch", zox_tst_guard_pitch_ready, zox_tst_guard_pitch_check, 120);
}
