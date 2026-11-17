// The XR camera flown in free roam: the sticks move and turn it, the headset turns and moves the head on top
//  - free_roam_step (cameras/fun/free_roam.c): the left stick moves where the head looks, the right turns the body
//  - the roam is the play space's origin in the world (its body); the head is tracked in it: turning the head and
//    walking in the real room (head_position) move it about the body
//  - the eyes render from the body, each eye's pose on top (fun/eye_render.c), so they had the room already, but
//    the camera entity sat at the body: streaming, sound and raycasts stayed at the room's centre as you walked.
//    It's at the head now (xr_roam_head), where a player's character follows too (doc/plans/xr.md step 5)
//  - the stick turns about the head, not the room's centre (walked away from it, a turn swung you around it)
//  - the same for a headset (openxr/render.c) and the simulator (sim/, no room: its head stays at the centre)
//  - in a game the character is the body instead (fun/body.c): the eyes ride it, the sticks move it as a gamepad
//  - the camera moved by something else (a game's start places it at the spawn, a teleport): the roam takes it up,
//    the head where it was put
static free_roam xr_roam;
static byte xr_roam_started = 0;
static float3 xr_roam_written;     // where it last put the camera

static inline float4x4 xr_roam_body_matrix() {
    return float4x4_transform(xr_roam.position, free_roam_rotation(&xr_roam));
}

// the head in the world: the body, then the head's place in the room
static inline float3 xr_roam_head(const free_roam* roam, float3 head_position) {
    return float3_add(roam->position, float4_rotate_float3(free_roam_rotation(roam), head_position));
}

// a step of the sticks, turning about the head (the room swings around it), moving along where it looks
//  - free_roam_step turns the body and moves it by the stick; the body then shifts by the head's offset turned
//    before less after, so the head is where it was plus the stick's move
static inline void xr_roam_step(free_roam* roam, float2 move, float2 turn, float4 head_rotation, float3 head_position, float delta) {
    const float3 offset_before = float4_rotate_float3(free_roam_rotation(roam), head_position);
    free_roam_step(roam, move, turn, head_rotation, delta);
    const float3 offset_after = float4_rotate_float3(free_roam_rotation(roam), head_position);
    roam->position = float3_add(roam->position, float3_sub(offset_before, offset_after));
}

// the eyes' body this frame: on the character in a game (fun/body.c), else the roam flown by the sticks
static float4x4 xr_view_body;

// the eyes kept shut (huds3_dark_hold) entering a game until the camera's on the body: loaded, the camera teleports
//  to it (a pop, play test); once on it this game, never again (a body gone later doesn't black the view)
static byte xr_body_seen = 0;

static inline void xr_dark_hold_update(ecs* world, byte on_body) {
    const entity player = zox_players[0];
    const entity game = zox_valid(player) ? zox_get_link(world, player, GameLink) : 0;
    const byte state = zox_valid(game) ? zox_getv(game, GameState) : zox_game_none;
    if (state == zox_game_none || state == zox_game_load_start || state >= zox_game_state_the_end) {
        xr_body_seen = 0;
    }
    xr_body_seen |= on_body;
    huds3_dark_hold = !on_body && !xr_body_seen && state >= zox_game_load_start && state <= zox_game_state_playing;
}

static void xr_roam_update(ecs* world, entity camera, float2 move, float2 turn, float4 head_rotation, float3 head_position, float delta) {
    xr_head_now_rotation = head_rotation;
    xr_head_now_position = head_position;
    if (xr_body_view(world, camera, head_rotation, head_position, &xr_view_body)) {
        xr_roam_started = 0;    // out of the game, the roam takes the camera up where it's left
        xr_dark_hold_update(world, 1);
        return;
    }
    xr_dark_hold_update(world, 0);
    xr_view_scale = 1;
    if (xr_roam_started && zox_valid(camera) && !float3_equals(zox_getv(camera, Position3D), xr_roam_written)) {
        xr_roam_started = 0;
    }
    if (!xr_roam_started && zox_valid(camera)) {
        // the head where the camera was put
        xr_roam.euler = quaternion_to_euler(zox_getv(camera, Rotation3D));
        xr_roam.position = float3_sub(zox_getv(camera, Position3D),
            float4_rotate_float3(free_roam_rotation(&xr_roam), head_position));
        xr_roam_started = 1;
    }
    xr_roam_step(&xr_roam, move, turn, head_rotation, head_position, delta);
    xr_view_body = xr_roam_body_matrix();
    if (zox_valid(camera)) {
        const float3 head = xr_roam_head(&xr_roam, head_position);
        zox_setv(camera, Position3D, head);
        xr_roam_written = head;
        zox_setv(camera, Rotation3D, quaternion_rotate(free_roam_rotation(&xr_roam), head_rotation));
    }
}
