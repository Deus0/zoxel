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
//  - xr_head_now_*: the head's latest tracked pose, written by a headset (openxr/render.c) or the simulator (sim/)
float4 xr_head_now_rotation = { 0, 0, 0, 1 };
float3 xr_head_now_position = { 0, 0, 0 };
static float3 xr_body_room_last;        // the head's x z in the room, last frame
static byte xr_body_room_started = 0;

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

// the body's facing in the tracking space's terms: the character's, turned around (its +z, the headset's -z)
static inline float4 xr_body_rotation(ecs* world, entity character) {
    return quaternion_rotate(zox_getv(character, Rotation3D), xr_body_turn_around());
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
//  - the room's x z went to the character (as far as walls let it, xr_room_walk_system), so it's taken off: the
//    eyes stay with the body
static byte xr_body_frame_get(ecs* world, entity camera, float4 head_rotation, float3 head_position, xr_body_frame* frame) {
    const entity head = xr_body_head(world, camera);
    const entity character = xr_body_character(world);
    if (!zox_valid(head) || !zox_valid(character)) {
        return 0;
    }
    const skeleton_now now = skeleton_now_get(world, get_skeleton(world, character));
    const float4 rotation = xr_body_rotation(world, character);
    const float scale = xr_body_scale(world, character);
    const float3 room = { head_position.x * scale, 0, head_position.z * scale };
    const float3 neck = skeleton_now_point(&now, zox_getv(head, Position3D));
    // the eyes' offset from the neck, in the head's space (+z its face), turned to the tracking space's
    const float3 offset = float4_rotate_float3(quaternion_inverse(zox_getv(head, Rotation3D)),
        float3_sub(zox_getv(camera, Position3D), zox_getv(head, Position3D)));
    const float3 eyes = float4_rotate_float3(quaternion_rotate(rotation, head_rotation),
        float4_rotate_float3(xr_body_turn_around(), offset));
    frame->position = float3_sub(float3_add(neck, eyes), float4_rotate_float3(rotation, room));
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

// the room's walk moves the character through physics: the head's x z step since the last frame, turned to the
//  body's facing, as velocity over this frame's dt (the one physics steps it by), added to the stick's (set first,
//  controllers3/sys/movement.c player3d_move_direct); a tracking jump (lost, recentred) isn't a step
void xr_room_walk_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const entity character = xr_body_character(world);
    if (!zox_xr_active || !zox_valid(character) || !xr_body_head(world, xr_camera)) {
        xr_body_room_started = 0;
        return;
    }
    const float3 room = { xr_head_now_position.x, 0, xr_head_now_position.z };
    float3 step = float3_zero;
    if (xr_body_room_started) {
        step = float4_rotate_float3(xr_body_rotation(world, character),
            float3_scale(float3_sub(room, xr_body_room_last), xr_body_scale(world, character)));
        if (float3_length(step) > 0.5f) {
            step = float3_zero;
        }
    }
    xr_body_room_last = room;
    xr_body_room_started = 1;
    const float dt = zox_delta_time;
    if (dt > 0 && (step.x != 0 || step.z != 0) && !zox_has(character, DisableMovement)) {
        Velocity3D* velocity = zox_getm(character, Velocity3D);
        const float scale = physics3_drag_compensation(world, character) / dt;
        velocity->value.x += step.x * scale;
        velocity->value.z += step.z * scale;
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
    const float4 around = xr_body_turn_around();
    zox_getm(head, LocalRotation3D)->value = quaternion_rotate(quaternion_rotate(around, xr_head_now_rotation), around);
    zox_sys_on_end();
} zoxd_system(xr_head_bone_system);

void zox_systems_xr_body(ecs* world) {
    // after the stick's velocity (controllers, imported before), before physics
    zox_system_1(xr_room_walk_system, zoxp_update, 0);
    zox_system_1(xr_head_bone_system, zoxp_post_update, 0);
}
