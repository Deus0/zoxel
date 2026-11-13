// tst: head_camera, the first person camera sits at the face: screenshots looking down and up
//  (tst/head_down.ppm, tst/head_up.ppm) to look at, the head's own voxels shouldn't fill the view
//  - run: bash bsh/tst.sh head_camera (no hud: tst.sh adds --nohud)
byte zox_tst_head_camera_stage = 0;
double zox_tst_head_camera_time = 0;

static inline void zox_tst_head_camera_pitch(ecs* world, float pitch) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity skeleton = get_skeleton(world, character);
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    const entity head_camera = zox_valid(head) ? zox_get_child_by_id(world, head, zox_id(Camera)) : 0;
    if (zox_valid(head_camera)) {
        // as mouse look does (controllers3/sys/rotation.c), the camera turned round (is_camera_positive_z)
        zox_setv(head_camera, LocalRotation3D, euler_to_quaternion((float3) { pitch, is_camera_positive_z ? M_PI : 0, 0 }));
    }
}

byte zox_tst_head_camera_ready(ecs* world) {
    if (!zox_tst_head_camera_stage) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_head_camera_stage = 1;
        zox_tst_head_camera_time = zox_current_time;
    }
    zox_tst_waiting = "the head camera screenshots";
    zox_tst_progress(zox_tst_head_camera_stage);
    if (zox_screenshot_request[0]) {
        return 0;
    }
    const float pitch = zox_tst_head_camera_stage <= 2 ? 1.2f : -1.2f;
    zox_tst_head_camera_pitch(world, pitch);
    if (zox_current_time - zox_tst_head_camera_time < 1) {
        return 0;
    }
    zox_tst_head_camera_time = zox_current_time;
    if (zox_tst_head_camera_stage == 1) {
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("head_down.ppm"));
    } else if (zox_tst_head_camera_stage == 3) {
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("head_up.ppm"));
    } else if (zox_tst_head_camera_stage == 4) {
        return 1;
    }
    zox_tst_head_camera_stage++;
    return 0;
}

void zox_tst_head_camera_check(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity skeleton = get_skeleton(world, character);
    const entity head = zox_get_link(world, skeleton, HeadBoneLink);
    const entity camera = zox_get_link(world, character, CameraLink);
    const float3 hs = zox_getv(head, BoneSize), hp = zox_getv(head, Position3D), cp = zox_getv(camera, Position3D), cl = zox_getv(camera, LocalPosition3D);
    const float4 r = zox_getv(character, Rotation3D);
    zox_log("[tst] head size [%.3f %.3f %.3f] at [%.3f %.3f %.3f] camera local [%.3f %.3f %.3f] at [%.3f %.3f %.3f] parent [%s] rot [%.2f %.2f %.2f %.2f] scale [%.3f]", hs.x, hs.y, hs.z, hp.x, hp.y, hp.z, cl.x, cl.y, cl.z, cp.x, cp.y, cp.z, zox_getn(zox_get_parent(world, camera)), r.x, r.y, r.z, r.w, zox_getv(character, BlockScale));
    zox_tst_check(zox_tst_clouds_share(zox_tst_path("head_down.ppm")) >= 0, "no screenshot looking down");
    zox_tst_check(zox_tst_clouds_share(zox_tst_path("head_up.ppm")) >= 0, "no screenshot looking up");
}
