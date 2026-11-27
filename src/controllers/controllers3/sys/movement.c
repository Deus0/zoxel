// A player's input to its character's move (doc/plans/movement.md): its devices (WASD, a stick, run, crouch) as the
//  way it wants to go (MoveDirection) and face (FaceDirection), and Running; the move (move.c) walks it by those
//  - the stick through its camera: first person along the character's facing (the mouse turns it, rotation.c), third
//    person along the orbit camera's (fun/third_person.c), topdown and ortho along their camera's
//  - not playing, its movement off, a free camera: it wants nothing (it would walk on, on the last input)
//  - crouching it sets itself (the skeleton's CrouchState); a headset walks without inertia, on its own (below)


// a headset: the stick moves the view at once at its top speed (xr_stick_velocity, body.c adds it to the view); the
//  body's move is that and a push toward the view's lead on it (xr_stick_lead), so it walks after the view and back
//  under it when it stops, by the move's physics (inertia, collisions, the speed stat)
static inline void player3_move_xr(ecs* world, entity character, MoveDirection* move, float2 stick) {
    const float4 frame = zox_getv(character, Rotation3D);
    const float2 top = character3_top_speed(world, character);
    const float3 local = player3_move_local(stick);
    xr_stick_velocity = physics3_flatten(float4_rotate_float3(frame, (float3) { local.x * top.x, 0, local.z * top.y }),
        character_up(world, character));
    const float3 wanted = float3_add(xr_stick_velocity, float3_scale(xr_stick_lead, xr_stick_catch));
    const float3 in_frame = float4_rotate_float3(float4_inverse(frame), wanted);
    move->value = float4_rotate_float3(frame, (float3) { top.x > 0 ? in_frame.x / top.x : 0, 0, top.y > 0 ? in_frame.z / top.y : 0 });
}

void player3d_move_system(iter* it) {
    zox_sys_on_begin();
    init_delta_time();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        entity character = zox_get_link(world, e, CharacterLink);
        if (!zox_valid(character) || !zox_has(character, Character3) || !zox_has(character, MoveDirection)) {
            continue;
        }
        // written in place, once a frame: two deferred sets in one pass lost the stick's (tst walk_bob stood still)
        MoveDirection* move = zox_getm(character, MoveDirection);
        move->value = float3_zero;      // wants nothing unless its input says (below)
        if (zox_xr_active) {
            xr_stick_velocity = float3_zero;
        }
        if (state->value != zox_player_state_playing || zox_has(character, DisableMovement)) {
            continue;
        }
        entity camera = zox_get_link(world, character, CameraLink);
        byte camera_mode = zox_valid(camera) ?
            zox_getv(camera, CameraState) :
            zox_camera_state_first_person;
        if (camera_mode == zox_camera_state_free) {
            continue;
        }
        byte is_running = 0;
        byte crouch_held = 0;       // the left control key held
        byte crouch_toggled = 0;    // a gamepad's left stick pushed in this frame
        float2 left_stick = float2_zero;
        entity devices[zox_children_capacity];
        uint length = zox_get_children_by_id(
            world,
            e,
            devices,
            zox_children_capacity,
            zox_id(Device));
        for (uint j = 0; j < length; j++) {
            entity e2 = devices[j];
            if (zox_has(e2, Disabled)) {
                continue;
            }
            uint children_capacity = zox_children_capacity;
            entity children[children_capacity];
            uint children_length = zox_get_children(
                world,
                e2,
                children,
                children_capacity);
            for (uint k = 0; k < children_length; k++) {
                entity e3 = children[k];
                if (zox_has(e3, Disabled)) {
                    continue;
                }
                // NOTE: Disabled for Fingers now
                if (zox_has(e3, Finger)) {
                    continue;
                }
                byte type = zox_getv(e3, DeviceButtonType);
                if (zox_has(e3, ZeviceStick)) {
                    if (type == zox_device_stick_left) {
                        float2 stick = zox_getv(e3, ZeviceStick);
                        left_stick.y += stick.y;
                        // NOTE: We must invert our Left Stick X Axis
                        left_stick.x += -stick.x;
                    }
                }
                if (zox_has(e3, ZeviceButton)) {
                    if (type == zox_btn_x) {
                        const ZeviceButton* zeviceButton = zox_get(e3, ZeviceButton);
                        if (!is_running && devices_get_pressed(zeviceButton->value)) {
                            is_running = 1;
                        }
                    } else if (type == zox_btn_left_stick_push && devices_get_pressed_this_frame(zox_getv(e3, ZeviceButton))) {
                        crouch_toggled = 1;
                    }
                }
            }
            if (zox_has(e2, Keyboard) && !zox_dbg_touch_with_mouse) {
                const Keyboard* keyboard = zox_get(e2, Keyboard);
                if (keyboard->s.is_pressed) left_stick.y += -1;
                if (keyboard->w.is_pressed) left_stick.y += 1;
                if (keyboard->a.is_pressed) left_stick.x += -1;
                if (keyboard->d.is_pressed) left_stick.x += 1;
                if (keyboard->left_shift.is_pressed) is_running = 1;
                if (keyboard->left_control.is_pressed) crouch_held = 1;
            }
        }
        // crouching: held by the left control key, toggled by a gamepad's left stick pushed in (holding a stick in
        //  while walking is awkward); the skeleton bends to it (bones/sys/arm_control.c), slower, no running. A
        //  headset crouches by the player's own body (xr/fun/body.c)
        byte crouching = 0;
        const entity skeleton = get_skeleton(world, character);
        if (!zox_xr_active && zox_valid(skeleton) && zox_has(skeleton, CrouchState)) {
            CrouchState* crouch = zox_getm(skeleton, CrouchState);
            byte value = crouch_held ? (crouch->value | 1) : (crouch->value & ~1);
            if (crouch_toggled) {
                value ^= 2;
            }
            crouch->value = value;
            crouching = value != 0;
        }
        if (crouching) {
            is_running = 0;
        }
        // a headset walks without inertia: the stick sets the ground velocity, at speed the frame it's pushed,
        //  stopped the frame it's let go (accelerating up and friction coasting down felt like lag, and sickens)
        if (zox_xr_active && !zox_getv(character, FlyMode)) {
            player3_move_xr(world, character, move, left_stick);
            continue;
        }
        if (is_running && !zox_has(character, Running)) {
            zox_add(character, Running);
        } else if (!is_running && zox_has(character, Running)) {
            zox_remove(character, Running);
        }
        // the frame the stick moves in: its camera's (and it turns to face that way), else its own facing
        const byte camera_facing = zox_valid(camera) && (camera_mode == zox_camera_state_topdown ||
            camera_mode == zox_camera_state_ortho || camera_mode == zox_camera_state_third_person);
        if (!camera_facing) {
            zox_setv(character, FaceDirection, float3_zero);
        }
        if (left_stick.x == 0 && left_stick.y == 0) {
            continue;
        }
        float4 frame = zox_getv(character, Rotation3D);
        if (camera_facing) {
            const float4 camera_rotation = zox_getv(camera, Rotation3D);
            if (camera_mode == zox_camera_state_third_person) {
                frame = third_person_move_rotation(camera_rotation);   // fun/third_person.c
            } else {
                float3 camera_euler = quaternion_to_euler(camera_rotation);
                camera_euler.x = 0;
                camera_euler.z = 0;
                frame = euler_to_quaternion(camera_euler);
            }
            zox_setv(character, FaceDirection, float4_rotate_float3(frame, (float3) { 0, 0, is_camera_positive_z ? 1 : -1 }));
        }
        move->value = float4_rotate_float3(frame, player3_move_local(left_stick));
    }
    zox_sys_on_end();
} zoxd_system(player3d_move_system);
