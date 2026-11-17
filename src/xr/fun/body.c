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
//  - the spine follows the real neck (xr_spine_solve): the chest's line, hips to neck, turned at the neck
//    (SpineLean, bones/ik sys/spine.c), the hips shifted (HipsShift): a neck closer to the hips than the line is long
//    bends, the hips back and the chest pitched down (to xr_bend_angle_max), then the rest crouches, the hips down
//    (the knees bend); one farther leans the line its way (to xr_lean_angle_max); the feet planted (PlantFeet,
//    sys/feet.c) under all of it; the neck still past the body's reach across the ground moves the body (the hips),
//    its feet stepping after; it leaned only by the neck's x z (bending down read as leaning forward, the body walked
//    forward, the drop crouched it), before that it slid the whole upright body (the eyes sank into the chest)
//  - the view rides the real neck: what the body can't reach (xr_body_view_off) moves the eyes the rest
//  - turning round: the body turns after the head, easing from xr_body_follow_start off it, at once past
//    xr_body_follow_angle (xr_body_follow), the room turned back on it (xr_body_room_turn) so the view stays; the feet
//    step round
//  - recentred (a headset's system button, xr_recentred): the body turns by the room's turn on it, the room straight,
//    the head straight on the body (it kept the old turn: the head off), the view still
//  - walking by the stick the walk's swing has the legs
//  - the real neck (xr_neck_room): the headset is the eyes, which swing about the player's neck as they look about
//    (down: forward and lower); the body's head bone swings the eyes about the character's neck already, so only
//    the neck's moves count (looking down slid the body forward, dropped the eyes into the chest)
//  - xr_head_now_*: the head's latest tracked pose, written by a headset (openxr/render.c) or the simulator (sim/)
float4 xr_head_now_rotation = { 0, 0, 0, 1 };
float3 xr_head_now_position = { 0, 0, 0 };
static float3 xr_body_hips_room;        // the body's hips in the room (x z, tracked, unscaled): the neck leans off them
static byte xr_body_room_started = 0;
float3 xr_body_view_off = { 0, 0, 0 };  // the real neck past where the body's reaches (the body's space, scaled)
#define xr_lean_angle_max 0.9f          // radians, the upper body leaning (the neck farther than the spine is long)
#define xr_bend_angle_max 1.9f          // radians, the upper body bending down (the neck closer)
#define xr_bend_back_max 0.45f          // of the spine's length, the hips back bending
#define xr_crouch_max 0.45f             // of the leg's length, the hips' drop crouching
#define xr_body_slack 0.02f             // the body's space, the real neck past the body's reach across the ground
                                        //  before the body moves after it
#define xr_body_view_off_max 0.1f       // the body's space, the most the view leaves the body's neck

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
#define xr_body_follow_angle 0.9f       // radians the head turns off the body: past it the body turns at once
#define xr_body_follow_start 0.35f      // radians the head turns off the body before the body eases after it
#define xr_body_follow_rate 4.0f        // per second, the body easing after the head

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

// the body turned about its up, the room turned back on it: the view still
static void xr_body_turn_by(ecs* world, entity character, float turn) {
    Euler* euler = zox_getm(character, Euler);
    euler->value.y += turn;
    zox_getm(character, Rotation3D)->value = quaternion_from_euler(euler->value);
    xr_body_room_turn = quaternion_normalize2(quaternion_rotate(quaternion_from_euler((float3) { 0, -turn, 0 }), xr_body_room_turn));
}

// the body turns after the head (the player turning round in the room): easing after it past xr_body_follow_start,
//  at once past xr_body_follow_angle; it never turned (the head bone twisted round alone), then only past 57 degrees
static void xr_body_follow(ecs* world, entity character, float delta) {
    if (!zox_has(character, Euler)) {
        return;
    }
    if (xr_recentred) {
        // the room's turn on the body to the body: the room straight on it, the head with it, the view still
        xr_recentred = 0;
        const float room_yaw = 2 * atan2f(xr_body_room_turn.y, xr_body_room_turn.w);
        xr_body_turn_by(world, character, room_yaw);
        xr_body_room_turn = quaternion_identity;
        xr_body_room_started = 0;   // the hips under the neck again
    }
    const float3 ahead = float4_rotate_float3(xr_body_head_local(xr_head_now_rotation), (float3) { 0, 0, 1 });
    if (fabsf(ahead.y) > 0.9f) {
        return;     // looking straight up or down: no facing to follow
    }
    const float yaw = atan2f(ahead.x, ahead.z);
    const float off = fabsf(yaw);
    if (off <= xr_body_follow_start) {
        return;
    }
    const float sign = yaw > 0 ? 1 : -1;
    float turn = (off - xr_body_follow_start) * float_min(1, delta * xr_body_follow_rate);
    if (off - turn > xr_body_follow_angle) {
        turn = off - xr_body_follow_angle;
    }
    xr_body_turn_by(world, character, sign * turn);
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
            spine_neck_local(world, &spine, zox_getv(skeleton, SpineLean), zox_getv(skeleton, HipsShift))));
    }
    // the eyes' offset from the neck, in the head's space (+z its face), turned to the tracking space's
    const float3 offset = float4_rotate_float3(quaternion_inverse(zox_getv(head, Rotation3D)),
        float3_sub(zox_getv(camera, Position3D), zox_getv(head, Position3D)));
    const float3 eyes = float4_rotate_float3(quaternion_rotate(rotation, head_rotation),
        float4_rotate_float3(xr_body_turn_around(), offset));
    // the real neck past where the body's reaches: the eyes go the rest
    neck = float3_add(neck, float4_rotate_float3(now.now_rotation, xr_body_view_off));
    const float3 off = float3_scale(head_position, -scale);
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

// the spine's pose for a neck, in the body's space from the hips' pivot at rest (line: the chest's line at rest, hips
//  to neck): its line (to the neck, as long as the line), the hips' shift; returns where its neck is
//  - the neck as far as the line is long: the line turned to it
//  - farther: the line leaned its way (to xr_lean_angle_max from up), short of it
//  - closer: bending, the hips back (away from the neck across the ground, behind if it's straight over them) to
//    xr_bend_back_max, then crouching, the hips down to crouch_max, the line pitched down to it (to
//    xr_bend_angle_max from up)
static float3 xr_spine_solve(float3 line, float3 neck, float crouch_max, float3* to_neck, float3* shift) {
    const float length = float3_length(line);
    const float3 across = { neck.x, 0, neck.z };
    const float out = float3_length(across);
    const float3 way = out > 0.0001f ? float3_scale(across, 1 / out) : (float3) { 0, 0, 1 };
    *shift = float3_zero;
    float max_angle = xr_lean_angle_max;
    if (float3_length(neck) < length) {
        float back = sqrtf(float_max(length * length - neck.y * neck.y, 0)) - out;
        back = back < 0 ? 0 : float_min(back, xr_bend_back_max * length);
        const float reach = out + back;
        float drop = sqrtf(float_max(length * length - reach * reach, 0)) - neck.y;
        drop = drop < 0 ? 0 : float_min(drop, crouch_max);
        *shift = (float3) { -way.x * back, -drop, -way.z * back };
        max_angle = xr_bend_angle_max;
    }
    // the line from the shifted hips to the neck, as long as it is, no farther from up than max_angle
    float3 to = float3_sub(neck, *shift);
    float3 direction = float3_length(to) > 0.0001f ? float3_normalize(to) : (float3) { 0, 1, 0 };
    const float angle = acosf(float_max(-1, float_min(1, direction.y)));
    if (angle > max_angle) {
        const float3 flat = { direction.x, 0, direction.z };
        const float3 side = float3_length(flat) > 0.0001f ? float3_normalize(flat) : way;
        direction = float3_add(float3_scale(side, sinf(max_angle)), (float3) { 0, cosf(max_angle), 0 });
    }
    *to_neck = float3_scale(direction, length);
    return float3_add(*shift, *to_neck);
}

// the room's walk: the spine follows the real neck (xr_spine_solve: leaning, bending, crouching), what it can't reach
//  across the ground moves the body (the hips) through physics (turned to the body's facing, as velocity over this
//  frame's dt, added to the stick's set first, controllers3/sys/movement.c player3d_move_direct), the feet stepping
//  after; what's left the view takes (xr_body_view_off); a tracking jump (lost, recentred) isn't a step
void xr_room_walk_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const entity character = xr_body_character(world);
    if (!zox_xr_active || !zox_valid(character) || !xr_body_head(world, xr_camera)) {
        xr_body_room_started = 0;
        xr_body_view_off = float3_zero;
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
    xr_body_follow(world, character, (float) zox_delta_time);
    const float scale = xr_body_scale(world, character);
    const float3 neck_room = xr_neck_room(xr_head_now_rotation, xr_head_now_position);
    const float3 room = { neck_room.x, 0, neck_room.z };
    if (!xr_body_room_started) {
        xr_body_hips_room = room;
    }
    xr_body_room_started = 1;
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
    // the chest's line, hips to neck, at rest (the chest's space is the skeleton's unleaned)
    const float3 line = float3_sub(zox_getv(spine.head, LocalPosition3D), zox_getv(spine.hips, LocalPosition3D));
    const float4 room_to_body = xr_body_room_to_body();
    float3 to_neck, shift, wanted, reached;
    float3 step = float3_zero;
    for (int i = 0; i < 3; i++) {
        // the real neck off the body's hips, in the body's space (scaled), from the hips' pivot at rest
        const float3 off = float4_rotate_float3(room_to_body,
            float3_scale((float3) { neck_room.x - xr_body_hips_room.x, neck_room.y, neck_room.z - xr_body_hips_room.z }, scale));
        wanted = float3_add(line, off);
        reached = xr_spine_solve(line, wanted, xr_crouch_max * leg, &to_neck, &shift);
        // past the body's reach across the ground: the hips move after the neck
        const float3 past = { wanted.x - reached.x, 0, wanted.z - reached.z };
        const float far = float3_length(past);
        if (far <= xr_body_slack) {
            break;
        }
        const float3 move_body = float3_scale(past, (far - xr_body_slack) / far);
        const float3 move_room = float3_scale(float4_rotate_float3(quaternion_inverse(room_to_body), move_body), 1 / scale);
        xr_body_hips_room = float3_add(xr_body_hips_room, (float3) { move_room.x, 0, move_room.z });
        step = float3_add(step, float4_rotate_float3(zox_getv(character, Rotation3D), move_body));
    }
    if (float3_length(step) > 0.5f) {
        step = float3_zero;     // a tracking jump: recentred under the neck
        xr_body_hips_room = room;
    }
    // what the body can't reach the view takes, so the eyes are where the player's are
    xr_body_view_off = float3_sub(wanted, reached);
    if (float3_length(xr_body_view_off) > xr_body_view_off_max) {
        xr_body_view_off = float3_scale(float3_normalize(xr_body_view_off), xr_body_view_off_max);
    }
    const float4 lean = quaternion_from_to(line, to_neck);
    if (zox_has(skeleton, SpineLean) && zox_has(skeleton, HipsShift)) {
        zox_getm(skeleton, SpineLean)->value = lean;
        zox_getm(skeleton, HipsShift)->value = shift;
    } else {
        zox_setv(skeleton, SpineLean, lean);
        zox_setv(skeleton, HipsShift, shift);
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
