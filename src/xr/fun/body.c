// XR in a game: the player's character is the body, the headset's pose on top of it
//  - in a game the xr camera is on the character's head bone (the first person attach, controllers3
//    head_camera.c): the sticks move and turn the character (as a gamepad's), so the roam doesn't fly it, and
//    never takes the camera up (the hierarchy writes it each frame: the roam took it up every frame, folding the
//    head's turn into the body's, the body fought the head)
//  - the eyes: the character's facing (its +z the tracking space's -z), at its neck (the head bone's pivot), the
//    head's turn swinging the eyes about it (the camera's offset from the head), the headset's pose on top
//  - walking in the real room (the head's tracked x z) moves the character through physics, with collisions: each
//    frame's step is its velocity for that frame (xr_room_walk_system, zoxp_update, before physics), so walls stop
//    it in the frame it's taken; a step a wall stops is still used up, the view stays with the body (no seeing
//    through walls, the room slides back instead)
//  - friction kept: the stick and room velocities are scaled against this frame's drag (physics3_drag_compensation),
//    so the body moves exactly as asked each frame; drag came and went with Grounded (it flickers on seams),
//    spiking the walk's speed: bumps
//  - the head bone turns as the headset does (xr_head_bone_system), the voxel head matches and the camera on it
//    (raycasts) looks where the head looks; its breathing bob is taken off (bobbing eyes sicken)
//  - leaning: the neck's x z off the hips leans the upper body about them (SpineLean, bones/ik sys/spine.c), as
//    far as xr_lean_angle_max; past it the body (the hips) moves after the neck, the feet planted (PlantFeet,
//    sys/feet.c) stepping under it; it moved the whole upright body with the neck, never leaning (bending forward
//    the eyes sank into the chest)
//  - crouching: the neck lower than the lean has it drops the hips (HipsDrop), the knees bend; past
//    xr_crouch_max the view goes lower itself (xr_body_view_drop)
//  - turning round: the body turns after the head past xr_body_follow_angle (xr_body_follow), the room turned back on
//    it (xr_body_room_turn) so the view stays; the feet step round
//  - walking by the stick the walk's swing has the legs
//  - the real neck (xr_neck_room): the headset is the eyes, which swing about the player's neck as they look about
//    (down: forward and lower); the body's head bone swings the eyes about the character's neck already, so only
//    the neck's moves count (looking down slid the body forward, dropped the eyes into the chest)
//  - xr_head_now_*: the head's latest tracked pose, written by a headset (openxr/render.c) or the simulator (sim/)
float4 xr_head_now_rotation = { 0, 0, 0, 1 };
float3 xr_head_now_position = { 0, 0, 0 };
static float3 xr_body_hips_room;        // the body's hips in the room (x z, tracked, unscaled): the neck leans off them
static byte xr_body_room_started = 0;
float xr_body_view_drop = 0;            // the neck lower than the body can crouch: the view goes the rest (scaled)
#define xr_lean_angle_max 0.7f          // radians, the upper body's lean before the body moves (steps)
#define xr_crouch_max 0.45f             // of the leg's length, the hips' drop crouching

// the eyes from the neck's pivot, in the head's space (-z ahead): up and forward of it
#define xr_neck_to_eyes ((float3) { 0, 0.10f, -0.08f })

// the player's neck in the room, from the head's tracked pose, from where it is with the head level at the origin
//  (OpenXR's local space starts there): still as the head turns about it
static inline float3 xr_neck_room(float4 head_rotation, float3 head_position) {
    return float3_add(float3_sub(head_position, float4_rotate_float3(head_rotation, xr_neck_to_eyes)), xr_neck_to_eyes);
}

static inline float4 xr_body_turn_around() {
    return quaternion_from_euler((float3) { 0, M_PI, 0 });
}

static entity xr_body_character(ecs* world) {
    const entity player = zox_players[0];
    return zox_valid(player) ? zox_get_link(world, player, CharacterLink) : 0;
}

// the head bone the camera rides on, 0 when it's not on one (menus: the roam flies)
static entity xr_body_head(ecs* world, entity camera) {
    const entity parent = zox_valid(camera) ? zox_get_parent(world, camera) : 0;
    return zox_valid(parent) && zox_has(parent, HeadBone) ? parent : 0;
}

// the tracking space's turn on the body: the body turning after the head (xr_body_follow) turns the room back, so
//  the world doesn't swing; a stick's turn turns the body and the room with it
float4 xr_body_room_turn = { 0, 0, 0, 1 };
#define xr_body_follow_angle 1.0f       // radians the head turns off the body before the body turns after it

// the room into the body's (the skeleton's) space: turned around (its +z, the headset's -z), and its turn on it
static inline float4 xr_body_room_to_body() {
    return quaternion_rotate(xr_body_turn_around(), xr_body_room_turn);
}

// the body's facing in the tracking space's terms: the character's, the room on it
static inline float4 xr_body_rotation(ecs* world, entity character) {
    return quaternion_rotate(zox_getv(character, Rotation3D), xr_body_room_to_body());
}

// the head bone's turn on the body: the headset's, from the room into the body, turned back to the bone's +z
static inline float4 xr_body_head_local(float4 head_rotation) {
    return quaternion_rotate(quaternion_rotate(xr_body_room_to_body(), head_rotation), xr_body_turn_around());
}

// the body turns after the head (the player turning round in the room): past xr_body_follow_angle off it, by as far
//  as it's past; the room turned back by it, the view still (it never turned: the head bone twisted round alone)
static void xr_body_follow(ecs* world, entity character) {
    const float3 ahead = float4_rotate_float3(xr_body_head_local(xr_head_now_rotation), (float3) { 0, 0, 1 });
    if (fabsf(ahead.y) > 0.9f || !zox_has(character, Euler)) {
        return;     // looking straight up or down: no facing to follow
    }
    const float yaw = atan2f(ahead.x, ahead.z);
    if (fabsf(yaw) <= xr_body_follow_angle) {
        return;
    }
    const float turn = yaw - (yaw > 0 ? xr_body_follow_angle : -xr_body_follow_angle);
    Euler* euler = zox_getm(character, Euler);
    euler->value.y += turn;
    zox_getm(character, Rotation3D)->value = quaternion_from_euler(euler->value);
    xr_body_room_turn = quaternion_normalize2(quaternion_rotate(quaternion_from_euler((float3) { 0, -turn, 0 }), xr_body_room_turn));
}

// the body the eyes and hands are on, in the world: the tracking space's origin and facing
typedef struct {
    float3 position;
    float4 rotation;
    float scale;        // tracked positions scaled to the body (fun/scale.c)
} xr_body_frame;

static inline float4x4 xr_body_frame_matrix(xr_body_frame frame) {
    return float4x4_transform(frame.position, frame.rotation);
}

// a pose tracked in the room (a controller's) into the world, on the body
static inline float3 xr_body_frame_point(xr_body_frame frame, float3 tracked) {
    return float3_add(frame.position, float4_rotate_float3(frame.rotation, float3_scale(tracked, frame.scale)));
}

// the body on the character, where it is now, returns whether the camera's on one
//  - the neck (the head bone's pivot) and the eyes' offset from it are where the hierarchy put them, carried to
//    where physics has moved the character since (skeleton_now), so it's right before the hierarchy runs too
//  - the eyes are the tracked eyes from the head's tracked place (their spread, turning with the head) on the
//    character's eyes (the head bone's swing about its neck), the neck leaned and dropped (the real neck's place,
//    xr_room_walk_system), lower still past the body's crouch
static byte xr_body_frame_get(ecs* world, entity camera, float4 head_rotation, float3 head_position, xr_body_frame* frame) {
    const entity head = xr_body_head(world, camera);
    const entity character = xr_body_character(world);
    if (!zox_valid(head) || !zox_valid(character)) {
        return 0;
    }
    const skeleton_now now = skeleton_now_get(world, get_skeleton(world, character));
    const float4 rotation = xr_body_rotation(world, character);
    const float scale = xr_body_scale(world, character);
    // the neck leaned and dropped as it is now (bones/ik sys/spine.c), else where the hierarchy left it
    const entity skeleton = get_skeleton(world, character);
    spine_bones spine;
    float3 neck = skeleton_now_point(&now, zox_getv(head, Position3D));
    if (zox_has(skeleton, SpineLean) && spine_bones_find(world, skeleton, &spine) && zox_has(spine.chest, RestPosition3D)) {
        neck = float3_add(now.now_position, float4_rotate_float3(now.now_rotation,
            spine_neck_local(world, &spine, zox_getv(skeleton, SpineLean), zox_getv(skeleton, HipsDrop))));
    }
    // the eyes' offset from the neck, in the head's space (+z its face), turned to the tracking space's
    const float3 offset = float4_rotate_float3(quaternion_inverse(zox_getv(head, Rotation3D)),
        float3_sub(zox_getv(camera, Position3D), zox_getv(head, Position3D)));
    const float3 eyes = float4_rotate_float3(quaternion_rotate(rotation, head_rotation),
        float4_rotate_float3(xr_body_turn_around(), offset));
    const float3 off = float3_sub((float3) { 0, -xr_body_view_drop, 0 }, float3_scale(head_position, scale));
    frame->position = float3_add(float3_add(neck, eyes), float4_rotate_float3(rotation, off));
    frame->rotation = rotation;
    frame->scale = scale;
    return 1;
}

// the eyes' body on the character, returns whether the camera's on one
static byte xr_body_view(ecs* world, entity camera, float4 head_rotation, float3 head_position, float4x4* body) {
    xr_body_frame frame;
    if (!xr_body_frame_get(world, camera, head_rotation, head_position, &frame)) {
        return 0;
    }
    *body = xr_body_frame_matrix(frame);
    xr_view_scale = frame.scale;
    return 1;
}

// how far the neck leans off the hips in the room (tracked, unscaled) before the body moves: the chest's line, hips
//  to neck, turned xr_lean_angle_max
static inline float xr_body_lean_max(ecs* world, const spine_bones* spine, float scale) {
    const float3 line = float3_sub(zox_getv(spine->head, LocalPosition3D), zox_getv(spine->hips, LocalPosition3D));
    return float3_length(line) * sinf(xr_lean_angle_max) / scale;
}

// the room's walk: the neck's x z off the body's hips leans its upper body (SpineLean) as far as xr_lean_angle_max,
//  past it the hips move after the neck through physics (turned to the body's facing, as velocity over this frame's
//  dt, added to the stick's set first, controllers3/sys/movement.c player3d_move_direct), the feet stepping under
//  them; the neck lower than the lean has it drops the hips (HipsDrop); a tracking jump (lost, recentred) isn't a step
void xr_room_walk_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const entity character = xr_body_character(world);
    if (!zox_xr_active || !zox_valid(character) || !xr_body_head(world, xr_camera)) {
        xr_body_room_started = 0;
        xr_body_view_drop = 0;
        xr_body_room_turn = quaternion_identity;
        return;
    }
    const entity skeleton = get_skeleton(world, character);
    spine_bones spine;
    if (!zox_valid(skeleton) || !spine_bones_find(world, skeleton, &spine)) {
        return;
    }
    if (!zox_has(skeleton, PlantFeet)) {
        zox_add(skeleton, PlantFeet);
    }
    xr_body_follow(world, character);
    const float scale = xr_body_scale(world, character);
    const float3 neck = xr_neck_room(xr_head_now_rotation, xr_head_now_position);
    const float3 room = { neck.x, 0, neck.z };
    // the chest's line, hips to neck, at rest (the chest's space is the skeleton's unleaned)
    const float3 line = float3_sub(zox_getv(spine.head, LocalPosition3D), zox_getv(spine.hips, LocalPosition3D));
    const float line_length = float3_length(line);
    const float lean_max = xr_body_lean_max(world, &spine, scale);
    if (!xr_body_room_started) {
        xr_body_hips_room = room;
    }
    xr_body_room_started = 1;
    float3 off = float3_sub(room, xr_body_hips_room);
    float3 step = float3_zero;
    const float leaning = float3_length(off);
    if (leaning > lean_max) {
        const float3 move = float3_scale(off, 1 - lean_max / leaning);
        xr_body_hips_room = float3_add(xr_body_hips_room, move);
        step = float4_rotate_float3(xr_body_rotation(world, character), float3_scale(move, scale));
        if (float3_length(step) > 0.5f) {
            step = float3_zero;     // a tracking jump: recentred under the neck
            xr_body_hips_room = room;
        }
        off = float3_sub(room, xr_body_hips_room);
    }
    // the lean: the chest's line turned so the neck is off the hips as the real one is (tracking -z is its +z)
    const float3 lean_off = float4_rotate_float3(xr_body_room_to_body(), float3_scale(off, scale));
    const float3 across = { line.x + lean_off.x, 0, line.z + lean_off.z };
    const float up = sqrtf(float_max(line_length * line_length - (across.x * across.x + across.z * across.z),
        0.04f * line_length * line_length));
    const float4 lean = quaternion_from_to(line, (float3) { across.x, up, across.z });
    // crouching: the neck lower than the lean has it
    float leg = 0.4f;
    {
        const BoneLinks* bones = zox_get(skeleton, BoneLinks);
        ik_leg legs;
        for (int i = 0; bones && i < bones->length; i++) {
            if (zox_valid(bones->value[i]) && zox_has(bones->value[i], ThighBone) && ik_leg_find(world, bones->value[i], &legs)) {
                leg = ik_leg_length(world, &legs);
                break;
            }
        }
    }
    const float crouch = up - (line.y + neck.y * scale);
    const float drop = crouch < 0 ? 0 : float_min(crouch, xr_crouch_max * leg);
    xr_body_view_drop = crouch > drop ? crouch - drop : 0;
    if (zox_has(skeleton, SpineLean)) {
        zox_getm(skeleton, SpineLean)->value = lean;
        zox_getm(skeleton, HipsDrop)->value = drop;
    } else {
        zox_setv(skeleton, SpineLean, lean);
        zox_setv(skeleton, HipsDrop, drop);
    }
    const float dt = zox_delta_time;
    if (dt > 0 && (step.x != 0 || step.z != 0) && !zox_has(character, DisableMovement)) {
        Velocity3D* velocity = zox_getm(character, Velocity3D);
        const float compensation = physics3_drag_compensation(world, character) / dt;
        velocity->value.x += step.x * compensation;
        velocity->value.z += step.z * compensation;
    }
    zox_sys_on_end();
} zoxd_system(xr_room_walk_system);

// the player's head bone turns as the headset: the body's facing, the head's turn, turned back to the bone's +z
//  - in the character's space (the chest and neck unturned): the turn around, the head's, the turn around back
//  - after the head's own systems (zoxp_update: head_look eases it to ahead), before the hierarchy
void xr_head_bone_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    if (!zox_xr_active) {
        return;
    }
    const entity character = xr_body_character(world);
    const entity skeleton = zox_valid(character) ? get_skeleton(world, character) : 0;
    const entity head = zox_valid(skeleton) ? zox_get_link(world, skeleton, HeadBoneLink) : 0;
    if (!zox_valid(head)) {
        return;
    }
    if (zox_has(head, OscillatePosition3D)) {
        zox_remove(head, OscillatePosition3D);
    }
    zox_getm(head, LocalRotation3D)->value = xr_body_head_local(xr_head_now_rotation);
    zox_sys_on_end();
} zoxd_system(xr_head_bone_system);

void zox_systems_xr_body(ecs* world) {
    // after the stick's velocity (controllers, imported before), before physics
    zox_system_1(xr_room_walk_system, zoxp_update, 0);
    zox_system_1(xr_head_bone_system, zoxp_post_update, 0);
}
