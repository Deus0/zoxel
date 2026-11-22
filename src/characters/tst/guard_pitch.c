// tst: guard_pitch, the raised guard looking up and down (bones/ik/sys/swing.c, ik/sys/arm_ik.c)
//  - raised, the first person camera pitched level, up 60, 75, 85, 89 and down 60 degrees, a second each, turned there in
//    mouse sized steps as mouse look turns it (camera_pitch_add)
//  - the head and the camera don't roll (their z): the head's right level with the body's, the camera's right level
//  - no part of the arm (its hand, elbow, the forearm's middle) in front of the camera crosses to the left of the
//    view's centre: the right arm stays on the right
//  - looking level and up the hand is in the camera's view, at every pitch 0.1 or more from the eyes (looking at the
//    sky it came in over the view's centre); at every pitch it's ahead of the head, not over and behind
//    it, and the elbow is bent (it locked straight looking up, the shoulder turned half round)
//  - looking up the body leans back: the forearm's middle in front of the camera, not beside it
//  - its hand a fist's: the palm in (left), the thumb up the view, at every pitch up (it tilted back at the view)
//  - from up 60 to up 85 the upper arm turns a little, not half a turn about itself (the shortest turn flipped it)
//  - run: bash bsh/tst.sh guard_pitch
#define zox_tst_gp_stages 6
const float zox_tst_gp_pitches[zox_tst_gp_stages] = { 0, 60, 85, -60, 75, 89 };   // degrees, up positive
double zox_tst_gp_start = 0;
float3 zox_tst_gp_seen[zox_tst_gp_stages];     // the hand in the camera's space (it faces -z)
float zox_tst_gp_ahead[zox_tst_gp_stages];     // the hand from the head along the body's level facing, of the reach
float zox_tst_gp_bend[zox_tst_gp_stages];      // the elbow's w: 1 straight
float4 zox_tst_gp_upper[zox_tst_gp_stages];    // the shoulder's world turn
float3 zox_tst_gp_elbow_seen[zox_tst_gp_stages];   // the elbow in the camera's space
float3 zox_tst_gp_forearm_seen[zox_tst_gp_stages]; // the forearm's middle in the camera's space
float zox_tst_gp_roll[zox_tst_gp_stages];      // the head's roll about its forward (z), degrees, from the body's
float zox_tst_gp_camera_roll[zox_tst_gp_stages];   // the camera's, its right from level
byte zox_tst_gp_measured = 0;
float3 zox_tst_gp_palm[zox_tst_gp_stages];     // the hand's palm in the camera's space (its rest +z)
float3 zox_tst_gp_thumb[zox_tst_gp_stages];    // its thumb (the right hand's rest -x)

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
    // the camera pitched as a mouse turns it (camera_pitch_add, controllers3/sys/rotation.c), in small steps toward
    //  the stage's pitch, so a roll its euler round trip picks up looking far up shows
    const float pitch = zox_tst_gp_pitches[stage] * degrees_to_radians;
    const float4 local = zox_getv(camera, LocalRotation3D);
    const float3 ahead = float4_rotate_float3(local, (float3) { 0, 0, -1 });
    const float now = asinf(float_max(-1, float_min(1, ahead.y)));
    const float step = float_min(2 * degrees_to_radians, fabsf(pitch - now));
    if (step > 0.0001f) {
        // which way is up for this camera: a small turn's effect on its view
        const float3 probed = float4_rotate_float3(camera_pitch_add(local, 0.01f, 89 * degrees_to_radians), (float3) { 0, 0, -1 });
        const float up_sign = probed.y > ahead.y ? 1 : -1;
        zox_setv(camera, LocalRotation3D, camera_pitch_add(local, up_sign * (pitch > now ? step : -step), 89 * degrees_to_radians));
    }
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
    const float4 camera_inverse = quaternion_inverse(zox_getv(camera, Rotation3D));
    zox_tst_gp_elbow_seen[stage] = float4_rotate_float3(camera_inverse, float3_sub(el, zox_getv(camera, Position3D)));
    zox_tst_gp_forearm_seen[stage] = float4_rotate_float3(camera_inverse,
        float3_sub(float3_scale(float3_add(el, hand), 0.5f), zox_getv(camera, Position3D)));
    // rolls: the head's right against the body's (level), and the camera's right off level (a tilted view)
    const float3 body_right = float4_rotate_float3(zox_getv(character, Rotation3D), float3_right);
    const float3 head_right = float4_rotate_float3(zox_getv(head, Rotation3D), float3_right);
    const float3 head_forward = float4_rotate_float3(zox_getv(head, Rotation3D), float3_forward);
    const float3 head_up = float4_rotate_float3(zox_getv(head, Rotation3D), float3_up);
    const float3 body_right_on = float3_normalize(float3_sub(body_right, float3_scale(head_forward, float3_dot(body_right, head_forward))));
    zox_tst_gp_roll[stage] = atan2f(float3_dot(body_right_on, head_up), float3_dot(body_right_on, head_right)) * radians_to_degrees;
    const float3 camera_right = float4_rotate_float3(zox_getv(camera, Rotation3D), float3_right);
    zox_tst_gp_camera_roll[stage] = asinf(float_max(-1, float_min(1, camera_right.y))) * radians_to_degrees;
    // the right hand's axes in the camera's space, as it rests in the anatomical position: its palm +z, its thumb -x
    //  (bones/ik ik_hand_rest_to_target)
    const float4 hand_seen = quaternion_rotate(camera_inverse, zox_getv(arm.bones[arm.length - 1], Rotation3D));
    zox_tst_gp_palm[stage] = float4_rotate_float3(hand_seen, float3_forward);
    zox_tst_gp_thumb[stage] = float4_rotate_float3(hand_seen, (float3) { -1, 0, 0 });
    zox_tst_gp_measured |= 1 << stage;
    return 0;
}

void zox_tst_guard_pitch_check(ecs* world) {
    (void) world;
    for (int i = 0; i < zox_tst_gp_stages; i++) {
        const float3 v = zox_tst_gp_seen[i];
        zox_log("[tst] guard_pitch [%.0f]: hand in view [%.3f %.3f %.3f], ahead of the head [%.2f] of reach, elbow w [%.3f]",
            zox_tst_gp_pitches[i], v.x, v.y, v.z, zox_tst_gp_ahead[i], zox_tst_gp_bend[i]);
        const float3 ev = zox_tst_gp_elbow_seen[i], fv = zox_tst_gp_forearm_seen[i];
        zox_log("[tst] guard_pitch [%.0f]: elbow in view [%.3f %.3f %.3f] forearm [%.3f %.3f %.3f], head roll [%.1f] camera roll [%.1f]",
            zox_tst_gp_pitches[i], ev.x, ev.y, ev.z, fv.x, fv.y, fv.z, zox_tst_gp_roll[i], zox_tst_gp_camera_roll[i]);
        const float3 pa = zox_tst_gp_palm[i], th = zox_tst_gp_thumb[i];
        zox_log("[tst] guard_pitch [%.0f]: palm [%.2f %.2f %.2f] thumb [%.2f %.2f %.2f] in view",
            zox_tst_gp_pitches[i], pa.x, pa.y, pa.z, th.x, th.y, th.z);
        // a fist's hand (a punch's, a place's): the palm in (left, the right hand's), the thumb up; looking up its palm
        //  tilted back at the view, a hand up against the sun
        if (zox_tst_gp_pitches[i] >= 0 && (zox_tst_gp_measured & (1 << i))) {
            // the palm in (the view's left), the thumb up (the guard's turned a little in): a right fist, not its
            //  palm up at the view, nor a left hand's turn
            zox_tst_check(pa.x < -0.8f && th.y > 0.6f, "looking [%.0f] degrees up the palm faces [%.2f %.2f %.2f], the thumb [%.2f %.2f %.2f], not in and up",
                zox_tst_gp_pitches[i], pa.x, pa.y, pa.z, th.x, th.y, th.z);
        }
        if (!(zox_tst_gp_measured & (1 << i))) {
            zox_tst_check(0, "the guard at [%.0f] degrees was never measured", zox_tst_gp_pitches[i]);
            continue;
        }
        // never so close to the eyes it fills the view (it came in to 0.05 looking at the sky, over the centre)
        zox_tst_check(float3_length(v) > 0.1f, "looking [%.0f] degrees up the hand is [%.3f] from the eyes, filling the view",
            zox_tst_gp_pitches[i], float3_length(v));
        if (zox_tst_gp_pitches[i] >= 0) {
            // ahead (-z), on a 16:9 screen: looking up the shoulder turns the arm up with the look
            zox_tst_check(v.z < 0 && fabsf(v.x) < -v.z * 1.6f && fabsf(v.y) < -v.z,
                "looking [%.0f] degrees up the hand is out of view [%.3f %.3f %.3f]", zox_tst_gp_pitches[i], v.x, v.y, v.z);
        }
        // looking up the body leans back (bones/ik sys/spine.c look_lean_system): the forearm comes up in front of
        //  the camera, not in level with it from the side (its middle was at z -0.005)
        if (zox_tst_gp_pitches[i] > 0) {
            zox_tst_check(zox_tst_gp_forearm_seen[i].z < -0.015f, "looking [%.0f] degrees up the forearm's middle is beside the camera [%.3f], not in front",
                zox_tst_gp_pitches[i], zox_tst_gp_forearm_seen[i].z);
        }
        zox_tst_check(zox_tst_gp_ahead[i] > 0, "looking [%.0f] degrees up the hand is behind the head [%.2f]",
            zox_tst_gp_pitches[i], zox_tst_gp_ahead[i]);
        zox_tst_check(zox_tst_gp_bend[i] < 0.995f, "looking [%.0f] degrees up the elbow is straight [%.3f]",
            zox_tst_gp_pitches[i], zox_tst_gp_bend[i]);
    }
    for (int i = 0; i < zox_tst_gp_stages; i++) {
        zox_tst_check(fabsf(zox_tst_gp_roll[i]) < 1 && fabsf(zox_tst_gp_camera_roll[i]) < 1,
            "looking [%.0f] degrees up the head rolled [%.1f] degrees, the camera [%.1f]", zox_tst_gp_pitches[i],
            zox_tst_gp_roll[i], zox_tst_gp_camera_roll[i]);
        const float3 points[3] = { zox_tst_gp_seen[i], zox_tst_gp_elbow_seen[i], zox_tst_gp_forearm_seen[i] };
        int crossed = 0;
        for (byte k = 0; k < 3; k++) {
            crossed += points[k].z < 0 && points[k].x < 0;
        }
        zox_tst_check(!crossed, "looking [%.0f] degrees up [%i] of the arm's points are left of the view's centre",
            zox_tst_gp_pitches[i], crossed);
    }
    const float4 a = zox_tst_gp_upper[1], b = zox_tst_gp_upper[2];
    const float turned = 2 * acosf(float_min(1, fabsf(a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w))) * radians_to_degrees;
    zox_log("[tst] guard_pitch: the upper arm turned [%.1f] degrees from up 60 to up 85", turned);
    zox_tst_check(turned < 30, "from up 60 to up 85 the upper arm turned [%.1f] degrees, flipped about itself", turned);
}

void zox_tsts_guard_pitch(ecs* world) {
    zox_tst_add("guard_pitch", zox_tst_guard_pitch_ready, zox_tst_guard_pitch_check, 120);
    zox_tst_set_setup("guard_pitch", zox_tst_void_setup, 1);    // the body alone, in the void
}
