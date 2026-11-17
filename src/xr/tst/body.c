// tst: xr_body, in a game on the simulator the player's character is the body (fun/body.c)
//  - into a game as xr_play does, then: the head turned (the headset), the left stick held, walked in the room
//  - the head turn: the character's facing stays, its head bone turns, the camera (on it) looks where the head does
//  - the eyes ride the character: the view's eyes (the body, then the head) are at the camera every frame, and the
//    roam never takes the camera up (it did each frame, folding the head's turn into the body)
//  - walking in the room at a wall: the body stops at it (physics, collisions), not through
//  - the hands on their controllers: walking, a hand stays where it is on the body as standing (it trailed a frame)
//  - everything tracked is scaled to the body (its height over the player's): 0.3m walked in the room moves it 0.3m
//    scaled
//  - the left stick walks the character, at speed straight away and stopped straight away (no inertia), each
//    frame's move steady over its dt (friction on: its drag flickering with Grounded made bumps); walking 0.3m in the room moves it 0.3m
//  - the device mode is the controllers': they're enabled playing, the keyboard, mouse and touchscreen feeders
//    (on a headset Android's stray pointer events switched the mode off them, the stick didn't walk)
//  - run: bash bsh/tst.sh xr_body
int zox_tst_xr_body_frames = 0;
float zox_tst_xr_body_worst_eyes = 0;       // the view's eyes from the camera, worst
float zox_tst_xr_body_facing_turn = 0;      // the character's facing, turned by the head (should stay)
float zox_tst_xr_body_look = 0;             // the camera's look on the head's (1 the same)
float zox_tst_xr_body_walked = 0;           // by the stick
float zox_tst_xr_body_room = 0;             // by the room
float zox_tst_xr_body_started = 0;          // the ground speed 3 frames into the stick (no inertia: at speed)
float zox_tst_xr_body_stopped = 1;          // 3 frames after it's let go (stopped)
float zox_tst_xr_body_speed_min = 1000;     // each frame's move over its dt, walking: steady (no bumps)
float zox_tst_xr_body_speed_max = 0;
float zox_tst_xr_body_dt_last = 0;          // read at zoxp_dbg_end it's the next frame's dt: the last one's is this frame's
float3 zox_tst_xr_body_last;
float3 zox_tst_xr_body_hand_still;          // the right hand in the body's space, standing
byte zox_tst_xr_body_hand_seen = 0;
float zox_tst_xr_body_hand_walking = -1;    // walking, its worst shift from standing (a frame behind, the walk's step)
float zox_tst_xr_body_wall_moved = -1;      // walked 2m in the room at a wall 1m away: how far it got
byte zox_tst_xr_body_wall_built = 0;
float3 zox_tst_xr_body_wall_dir;
float4 zox_tst_xr_body_facing;
float3 zox_tst_xr_body_from;

byte zox_tst_xr_body_ready(ecs* world) {
    if (zox_tst_xr_play_stage < 3) {
        zox_tst_xr_play_ready(world);
        return 0;
    }
    const entity character = xr_body_character(world);
    if (!zox_valid(character) || !xr_body_head(world, xr_camera)) {
        zox_tst_waiting = "the xr camera on the character's head";
        return 0;
    }
    const int f = ++zox_tst_xr_body_frames;
    zox_tst_progress(f);
    zox_tst_waiting = "the head, stick and room";
    if (f > 60) {
        // the eyes in the view at the camera: the view's body, the head's centre on top (the sim's eyes straddle it)
        const float3 eyes = float4x4_get_position(float4x4_multiply(
            float4x4_transform(float3_scale(xr_sim_script_head_position, xr_view_scale), xr_head_now_rotation), xr_view_body));
        const float d = float3_distance(eyes, zox_getv(xr_camera, Position3D));
        // the stick and room steps move it a frame ahead of the hierarchy, so only while still
        if ((f < 130) && d > zox_tst_xr_body_worst_eyes) {
            zox_tst_xr_body_worst_eyes = d;
        }
    }
    {
        const entity shoulder = xr_hands_shoulder(world, get_skeleton(world, character), xr_hand_right);
        ik_arm arm;
        xr_body_frame frame;
        if (zox_valid(shoulder) && zox_has(shoulder, IKHeld) && ik_arm_find(world, shoulder, &arm) &&
            xr_body_frame_get(world, xr_camera, xr_head_now_rotation, xr_head_now_position, &frame)) {
            const float3 hand = float4_rotate_float3(quaternion_inverse(frame.rotation),
                float3_sub(zox_getv(arm.bones[arm.length - 1], Position3D), frame.position));
            if (f > 140 && f < 150) {
                zox_tst_xr_body_hand_still = hand;
                zox_tst_xr_body_hand_seen = 1;
            }
            if (f > 156 && f < 208 && zox_tst_xr_body_hand_seen) {
                zox_tst_xr_body_hand_walking = float_max(zox_tst_xr_body_hand_walking, float3_distance(hand, zox_tst_xr_body_hand_still));
            }
        }
    }
    if (f == 60) {
        zox_tst_xr_body_facing = zox_getv(character, Rotation3D);
    }
    if (f > 60 && f < 130) {
        xr_sim_head_euler.y = 0.8f;     // looked left
        xr_sim_head_euler.x = 0.3f;     // and up
    }
    if (f == 129) {
        const float4 now = zox_getv(character, Rotation3D);
        zox_tst_xr_body_facing_turn = 1 - fabsf(float4_dot(now, zox_tst_xr_body_facing));
        // the camera looks (-z) where the head does: the body, then the head
        const float3 look = float4_rotate_float3(zox_getv(xr_camera, Rotation3D), float3_backward);
        const float3 want = float4_rotate_float3(quaternion_rotate(xr_body_rotation(world, character), xr_head_now_rotation), float3_backward);
        zox_tst_xr_body_look = float3_dot(look, want);
        xr_sim_head_euler = float3_zero;
    }
    if (f == 149) {
        zox_tst_xr_body_from = zox_getv(character, Position3D);
    }
    if (f >= 150 && f < 210) {
        xr_sim_script_left_stick = (float2) { 0, 1 };
    }
    {
        const float3 p = zox_getv(character, Position3D);
        if (f >= 153 && f < 210 && zox_tst_xr_body_dt_last > 0) {
            const float3 d = float3_sub(p, zox_tst_xr_body_last);
            const float speed = sqrtf(d.x * d.x + d.z * d.z) / zox_tst_xr_body_dt_last;
            zox_tst_xr_body_speed_min = float_min(zox_tst_xr_body_speed_min, speed);
            zox_tst_xr_body_speed_max = float_max(zox_tst_xr_body_speed_max, speed);
        }
        zox_tst_xr_body_last = p;
        zox_tst_xr_body_dt_last = (float) zox_delta_time;
    }
    if (f == 153 || f == 213) {
        // the speed it moved this frame (its velocity is after drag: the next frame's is set again)
        const float speed = f == 153 ? zox_tst_xr_body_speed_max : sqrtf(zox_getv(character, Velocity3D).x * zox_getv(character, Velocity3D).x + zox_getv(character, Velocity3D).z * zox_getv(character, Velocity3D).z);
        if (f == 153) {
            zox_tst_xr_body_started = speed;
        } else {
            zox_tst_xr_body_stopped = speed;
        }
    }
    if (f == 210) {
        xr_sim_script_left_stick = float2_zero;
        const float3 moved = float3_sub(zox_getv(character, Position3D), zox_tst_xr_body_from);
        zox_tst_xr_body_walked = sqrtf(moved.x * moved.x + moved.z * moved.z);
    }
    if (f == 240) {
        zox_tst_xr_body_from = zox_getv(character, Position3D);
        xr_sim_script_head_position = (float3) { 0.3f, 0, 0 };
    }
    if (f == 250) {
        const float3 moved = float3_sub(zox_getv(character, Position3D), zox_tst_xr_body_from);
        zox_tst_xr_body_room = sqrtf(moved.x * moved.x + moved.z * moved.z);
        xr_sim_script_head_position = float3_zero;
    }
    // a wall: a pillar of bricks 1m ahead along the room's x, then the head walks 2m at it in the room
    if (f == 260) {
        const entity terrain = zox_get_link(world, character, TerrainLink);
        const entity realm = zox_get_link(world, character, RealmLink);
        const entity bricks = zox_valid(realm) ? zox_get_child_by_id(world, realm, zox_id(BlockBricks)) : 0;
        if (zox_valid(terrain) && zox_valid(bricks)) {
            const float scale = zox_getv(terrain, BlockScale);
            zox_tst_xr_body_wall_dir = float3_normalize(float4_rotate_float3(xr_body_rotation(world, character), (float3) { 1, 0, 0 }));
            zox_tst_xr_body_from = zox_getv(character, Position3D);
            // a slab 1m to 1.75m ahead, 2m wide, from under the feet to 2.5m up (clear of the body: one built
            //  around it walked it out through the wall)
            const byte index = zox_getv(bricks, BlockIndex);
            const float3 side = float3_cross(float3_up, zox_tst_xr_body_wall_dir);
            int placed = 0;
            for (float ahead = 1.0f; ahead <= 1.75f; ahead += scale * 0.5f) {
                for (float across = -1; across <= 1; across += scale * 0.5f) {
                    for (float up = -1; up <= 2.5f; up += scale * 0.5f) {
                        const float3 p = float3_add(zox_tst_xr_body_from, float3_add(float3_add(
                            float3_scale(zox_tst_xr_body_wall_dir, ahead), float3_scale(side, across)), (float3) { 0, up, 0 }));
                        placed += terrain_queue_voxel(world, terrain, real_position_to_block_position(p, scale), index, 0);
                    }
                }
            }
            zox_tst_xr_body_wall_built = placed > 0;
            zox_log("[tst] xr_body: a wall of [%i] bricks, block scale [%f]", placed, scale);
        }
    }
    if (f > 320 && f <= 420) {
        xr_sim_script_head_position = (float3) { (f - 320) * 0.02f, 0, 0 };
    }
    if (f == 440) {
        zox_tst_xr_body_wall_moved = float3_dot(float3_sub(zox_getv(character, Position3D), zox_tst_xr_body_from), zox_tst_xr_body_wall_dir);
        xr_sim_script_head_position = float3_zero;
    }
    return f > 440;
}

void zox_tst_xr_body_check(ecs* world) {
    for (byte i = 0; i < 2; i++) {
        zox_tst_check(zox_valid(xr_controllers[i]) && !zox_has(xr_controllers[i], Disabled), "the xr controller [%i] is disabled playing", i);
    }
    {
        entity devices[zox_children_capacity];
        const uint length = zox_get_children_by_id(world, zox_players[0], devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            zox_tst_check(zox_has(devices[j], XrController) || zox_has(devices[j], DeviceFeeder),
                "the device [%s] can switch the device mode off the xr controllers", zox_getn(devices[j]));
        }
    }
    zox_log("[tst] xr_body: eyes off the camera [%.4f], facing turned [%.5f], look [%.4f], walked [%.3f], room [%.3f]",
        zox_tst_xr_body_worst_eyes, zox_tst_xr_body_facing_turn, zox_tst_xr_body_look, zox_tst_xr_body_walked, zox_tst_xr_body_room);
    zox_tst_check(zox_tst_xr_body_worst_eyes < 0.02f, "the eyes left the camera by [%f]", zox_tst_xr_body_worst_eyes);
    zox_tst_check(zox_tst_xr_body_facing_turn < 0.0001f, "the head's turn turned the body [%f]", zox_tst_xr_body_facing_turn);
    zox_tst_check(zox_tst_xr_body_look > 0.99f, "the camera doesn't look where the head does [%f]", zox_tst_xr_body_look);
    zox_log("[tst] xr_body: speed 3 frames in [%.3f] of [%.3f], 3 frames after [%.3f]", zox_tst_xr_body_started, max_velocity3D.y, zox_tst_xr_body_stopped);
    zox_log("[tst] xr_body: walking speed per frame [%.4f to %.4f]", zox_tst_xr_body_speed_min, zox_tst_xr_body_speed_max);
    zox_tst_check(zox_tst_xr_body_speed_min > max_velocity3D.y * 0.97f && zox_tst_xr_body_speed_max < max_velocity3D.y * 1.03f,
        "walking speed bumped, [%f to %f] of [%f]", zox_tst_xr_body_speed_min, zox_tst_xr_body_speed_max, max_velocity3D.y);
    zox_tst_check(zox_tst_xr_body_started > max_velocity3D.y * 0.97f, "the stick ramped up, not at speed [%f]", zox_tst_xr_body_started);
    zox_tst_check(zox_tst_xr_body_stopped < 0.05f, "let go it coasted [%f]", zox_tst_xr_body_stopped);
    zox_tst_check(zox_tst_xr_body_walked > 0.3f, "the left stick didn't walk the character [%f]", zox_tst_xr_body_walked);
    zox_log("[tst] xr_body: walking, the right hand shifted on the body by [%.4f] from standing", zox_tst_xr_body_hand_walking);
    zox_tst_check(zox_tst_xr_body_hand_walking >= 0, "no right hand held on its controller");
    zox_tst_check(zox_tst_xr_body_hand_walking < 0.005f, "walking, the hand trails its controller by [%f]", zox_tst_xr_body_hand_walking);
    zox_log("[tst] xr_body: walked 2m in the room at a wall 1m away, got [%.3f]", zox_tst_xr_body_wall_moved);
    zox_tst_check(zox_tst_xr_body_wall_built, "no wall built");
    zox_tst_check(zox_tst_xr_body_wall_moved > 0.2f && zox_tst_xr_body_wall_moved < 0.95f, "the room's walk went [%f] at a wall 1m away", zox_tst_xr_body_wall_moved);
    // scaled to the body: the character's height over the player's (fun/scale.c)
    const float scale = xr_body_scale(world, xr_body_character(world));
    zox_log("[tst] xr_body: scale [%.3f] (the character [%.3f] tall, the player [%.2f])", scale, scale * xr_player_height, xr_player_height);
    zox_tst_check(scale > 0.4f && scale < 0.8f, "the scale [%f] isn't a 1m character's over a 1.7m player's", scale);
    zox_tst_check(fabsf(zox_tst_xr_body_room - 0.3f * scale) < 0.02f, "walking 0.3m in the room moved it [%f], not [%f]", zox_tst_xr_body_room, 0.3f * scale);
}

void zox_tsts_xr_body(ecs* world) {
    zox_tst_add("xr_body", zox_tst_xr_body_ready, zox_tst_xr_body_check, 150);
    zox_tst_set_setup("xr_body", zox_tst_xr_sim_setup, 1);
}
