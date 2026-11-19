// The eyes inside the body, for the XR body's tests (look_down, lean): each frame the eyes (on the body, fun/body.c)
//  against every part of it, each bone's box round its part's middle (BoneCentre, BoneSize half sizes) turned with
//  it, the head and hat too (the eyes sit a voxel in front of the face): inside one, the player sees into their own
//  body
//  - the eyes' near plane sees in before they're in: within zox_tst_xr_eyes_near of a box counts
#define zox_tst_xr_eyes_near 0.01f
float zox_tst_xr_eyes_worst = -99;      // the most the eyes were in a part, metres (under 0: out of them all)
char zox_tst_xr_eyes_part[64] = "";     // that part, and what the body was doing
int zox_tst_xr_eyes_inside = 0;         // frames inside one

// the eyes against the body this frame, while it's doing what (logged with the worst)
static void zox_tst_xr_eyes_measure(ecs* world, const char* doing) {
    const entity character = xr_body_character(world);
    const entity skeleton = zox_valid(character) ? get_skeleton(world, character) : 0;
    xr_body_frame frame;
    if (!zox_valid(skeleton) || !xr_body_frame_get(world, xr_camera, xr_head_now_rotation, xr_head_now_position, &frame)) {
        return;
    }
    const entity head = zox_get_link(world, skeleton, HeadBoneLink);
    const float3 eye = xr_body_frame_point(frame, xr_head_now_position);
    const float scale = zox_has(character, Scale1) ? zox_getv(character, Scale1) : 1;
    const BoneLinks* bones = zox_get(skeleton, BoneLinks);
    float worst = -99;
    entity worst_bone = 0;
    for (int i = 0; bones && i < bones->length; i++) {
        const entity bone = bones->value[i];
        if (!zox_valid(bone)) {
            continue;
        }
        const float4 rotation = zox_getv(bone, Rotation3D);
        const float3 centre = float3_add(zox_getv(bone, Position3D), float4_rotate_float3(rotation,
            float3_scale(float3_sub(zox_getv(bone, BoneCentre), zox_getv(bone, BonePosition)), scale)));
        const float3 local = float4_rotate_float3(quaternion_inverse(rotation), float3_sub(eye, centre));
        const float3 size = float3_scale(zox_getv(bone, BoneSize), scale);
        // in by the least of its axes (under 0: out)
        const float in = fminf(fminf(size.x - fabsf(local.x), size.y - fabsf(local.y)), size.z - fabsf(local.z));
        if (in > worst) {
            worst = in;
            worst_bone = bone;
        }
    }
    zox_tst_xr_eyes_inside += worst > -zox_tst_xr_eyes_near;
    if (worst > zox_tst_xr_eyes_worst) {
        zox_tst_xr_eyes_worst = worst;
        snprintf(zox_tst_xr_eyes_part, sizeof(zox_tst_xr_eyes_part), "%s %s", doing,
            !zox_valid(worst_bone) ? "?" : worst_bone == head ? "head" : zox_has(worst_bone, HipsBone) ? "hips" :
            zox_has(worst_bone, ShoulderBone) ? "shoulder" : zox_has(worst_bone, ArmBone) ? "arm" : zox_has(worst_bone, ThighBone) ? "thigh" :
            zox_get_parent(world, worst_bone) == skeleton ? "chest" : "a part");
    }
}

extern char zox_screenshot_request[256];

// what the eyes see, to tst/xr_eyes/<name>.ppm (both eyes side by side)
static inline void zox_tst_xr_eyes_screenshot(const char* name) {
    char path[128];
    snprintf(path, sizeof(path), "xr_eyes/%s.ppm", name);
    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(path));
}

static void zox_tst_xr_eyes_check(const char* test) {
    zox_log("[tst] %s: the eyes in the body [%i] frames, the most [%.3f] (%s)", test, zox_tst_xr_eyes_inside,
        zox_tst_xr_eyes_worst, zox_tst_xr_eyes_part);
    zox_tst_check(zox_tst_xr_eyes_inside == 0, "%s: the eyes went into the body [%i] frames, [%.3f] in (%s)", test,
        zox_tst_xr_eyes_inside, zox_tst_xr_eyes_worst, zox_tst_xr_eyes_part);
}
