// XR in a game: the player's character is the body, the headset's pose on top of it
//  - in a game the xr camera is on the character's head bone (the first person attach, controllers3
//    head_camera.c): the sticks move and turn the character (as a gamepad's), so the roam doesn't fly it, and
//    never takes the camera up (the hierarchy writes it each frame: the roam took it up every frame, folding the
//    head's turn into the body's, the body fought the head)
//  - the eyes: the character's facing (its +z the tracking space's -z), at its neck (the head bone's pivot), the
//    head's turn swinging the eyes about it (the camera's offset from the head), the headset's pose on top
//  - walking in the real room (the head's tracked x z) moves the character, so the body stays under the head
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

// the eyes' body on the character, returns whether the camera's on one; moves it by the room's walk
static byte xr_body_view(ecs* world, entity camera, float4 head_rotation, float3 head_position, float4x4* body) {
    const entity head = xr_body_head(world, camera);
    const entity character = xr_body_character(world);
    if (!zox_valid(head) || !zox_valid(character)) {
        xr_body_room_started = 0;
        return 0;
    }
    const float4 rotation = xr_body_rotation(world, character);
    // the room's walk since the last frame moves the character, the body under the head
    const float3 room = { head_position.x, 0, head_position.z };
    float3 step = float3_zero;
    if (xr_body_room_started) {
        step = float4_rotate_float3(rotation, float3_sub(room, xr_body_room_last));
        if (float3_length(step) > 0.5f) {
            step = float3_zero;     // a tracking jump (lost, recentred), not a step
        }
    }
    xr_body_room_last = room;
    xr_body_room_started = 1;
    if (step.x != 0 || step.z != 0) {
        Position3D* position = zox_getm(character, Position3D);
        position->value = float3_add(position->value, step);
    }
    // the neck, this frame's step on top (the hierarchy placed it before the step)
    const float3 neck = float3_add(zox_getv(head, Position3D), step);
    // the eyes' offset from the neck, in the head's space (+z its face), turned to the tracking space's
    const float3 offset = float4_rotate_float3(quaternion_inverse(zox_getv(head, Rotation3D)),
        float3_sub(zox_getv(camera, Position3D), zox_getv(head, Position3D)));
    const float3 eyes = float4_rotate_float3(quaternion_rotate(rotation, head_rotation),
        float4_rotate_float3(xr_body_turn_around(), offset));
    // the room's x z is the character's now, so it's taken off: the eyes stay at the neck as it walks
    const float3 position = float3_sub(float3_add(neck, eyes), float4_rotate_float3(rotation, room));
    *body = float4x4_transform(position, rotation);
    return 1;
}

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
    zox_system_1(xr_head_bone_system, zoxp_post_update, 0);
}
