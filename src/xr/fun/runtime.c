// The XR runtime: what a headset gives the engine, the same for OpenXR (openxr/runtime.c) and the simulator (sim/),
//  so the frame, the controllers and the eyes run through one set of systems (doc/plans/xr.md step 1)
//  - xr_frame_begin_system (zoxp_begin): the frame begun (a headset waits for its pace), the head located for the
//      frame's body, the culling set from the eyes
//  - xr_controllers_system (zoxp_inputs_extract): each hand's stick, buttons and poses into its controller device
//      - aim: where the hand points (the device's pose: rays at 3D ui)
//      - grip: where the hand holds the controller (the body's hands reach it: fun/hands.c)
//  - xr_render_system (zoxp_render): the head and eyes located again (the freshest), free roam, each eye drawn
//      (fun/eye_render.c), the frame ended
//  - the backend: a headset's (xr_runtime_now, set at boot with zox_xr), else the simulator's while zox_xr_sim is on
//      (--xr-sim, or a test's setup); neither, nothing runs
//  - poses are in the tracking space (the room), a hand's or eye's

// one eye this frame: in the head's space (the room's), its four angles, where it draws
typedef struct {
    float4x4 matrix;
    float4 fov;
    float2 viewport_position;
    float2 viewport_size;
} xr_eye_view;

typedef struct {
    const char* name;
    // the frame's start: returns whether a frame began (a headset's paced by xrWaitFrame); running cleared when
    //  the runtime's done (the session's over)
    byte (*frame_begin)(ecs* world, byte* running);
    // the head (between the eyes) and both eyes, located now; returns whether they were
    byte (*locate)(ecs* world, float4* head_rotation, float3* head_position, xr_eye_view eyes[2]);
    // the inputs, read once a frame before the hands: returns whether they can be
    byte (*inputs)(ecs* world);
    float2 (*stick)(ecs* world, byte hand);         // as OpenXR has it (x right, y up)
    byte (*button)(ecs* world, byte hand, byte type);
    byte (*aim)(ecs* world, byte hand, float3* position, float4* rotation);    // returns whether it's tracked
    byte (*grip)(ecs* world, byte hand, float3* position, float4* rotation);
    float (*delta)(ecs* world);                     // the frame's time step, the roam's
    byte (*should_render)(ecs* world);
    byte (*eye_begin)(ecs* world, byte eye);        // binds the eye's image, returns whether it can draw
    void (*eye_end)(ecs* world, byte eye);
    void (*frame_end)(ecs* world);
    byte flip_y;                                    // its images read top down (a headset's swapchains)
} xr_runtime;

const xr_runtime* xr_runtime_now = NULL;
static byte xr_runtime_frame_open = 0;
extern byte zox_xr_sim;
extern const xr_runtime xr_sim_runtime;

static inline const xr_runtime* xr_runtime_get() {
    return xr_runtime_now ? xr_runtime_now : (zox_xr_sim ? &xr_sim_runtime : NULL);
}

void xr_frame_begin_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const xr_runtime* runtime = xr_runtime_get();
    if (!runtime) {
        return;
    }
    byte running = 1;
    xr_runtime_frame_open = runtime->frame_begin(world, &running);
    if (xr_runtime_frame_open) {
        // the head for this frame's body (the head bone, the hands' shoulders), located again before the eyes draw
        float4 rotation;
        float3 position;
        xr_eye_view eyes[2];
        if (runtime->locate(world, &rotation, &position, eyes)) {
            xr_head_now_rotation = rotation;
            xr_head_now_position = position;
            xr_cull_set(world, eyes[0].fov, eyes[1].fov);
        }
    }
    if (!running) {
        zox_log("[XR] the runtime [%s] ended, ending the game", runtime->name);
        engine_end();
    }
    zox_sys_on_end();
} zoxd_system(xr_frame_begin_system);

// each hand into its controller: its zevices (stick, buttons), its aim (the device's pose), its grip (the body's hand)
void xr_controllers_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const xr_runtime* runtime = xr_runtime_get();
    if (!runtime || !runtime->inputs(world)) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(XrHand);
    zox_sys_out(DeviceHasInput);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(XrHand, hand_);
        zox_sys_o(DeviceHasInput, has_input);
        const byte hand = hand_->value == xr_hand_right ? xr_hand_right : xr_hand_left;
        const float2 stick = runtime->stick(world, hand);
        byte any = stick.x != 0 || stick.y != 0;
        iter it2 = zox_children(world, e);
        while (zox_children_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                const entity e2 = it2.entities[j];
                if (zox_has(e2, ZeviceStick)) {
                    xr_controller_stick_set(world, e2, stick);
                } else if (zox_has(e2, ZeviceButton)) {
                    const byte held = runtime->button(world, hand, zox_getv(e2, DeviceButtonType));
                    xr_controller_button(world, e2, held);
                    any |= held;
                }
            }
        }
        has_input->value = any;
        float3 position;
        float4 rotation;
        if (runtime->grip(world, hand, &position, &rotation)) {
            xr_hand_track(hand, position, rotation);
        }
        if (runtime->aim(world, hand, &position, &rotation)) {
            xr_controller_pose(world, e, position, rotation);
        }
    }
    zox_sys_on_end();
} zoxd_system(xr_controllers_system);

void xr_render_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const xr_runtime* runtime = xr_runtime_get();
    if (!runtime || !xr_runtime_frame_open) {
        return;
    }
    xr_runtime_frame_open = 0;
    const float delta = runtime->delta(world);
    float4 head_rotation;
    float3 head_position;
    xr_eye_view eyes[2];
    if (!runtime->locate(world, &head_rotation, &head_position, eyes)) {
        runtime->frame_end(world);
        return;
    }
    // the sticks drive the menus while one's open (fun/controller.c)
    const byte roaming = xr_roam_input_enabled(world);
    const float2 move = roaming ? xr_controller_stick(world, xr_hand_left) : float2_zero;
    const float2 turn = roaming ? xr_controller_stick(world, xr_hand_right) : float2_zero;
    xr_roam_update(world, xr_camera, move, turn, head_rotation, head_position, delta);
    if (runtime->should_render(world)) {
        const float4x4 body = xr_view_body;
        for (byte i = 0; i < 2; i++) {
            if (!runtime->eye_begin(world, i)) {
                continue;
            }
            xr_render_eye(world, eyes[i].matrix, eyes[i].fov, body, eyes[i].viewport_position, eyes[i].viewport_size, runtime->flip_y);
            runtime->eye_end(world, i);
        }
    }
    runtime->frame_end(world);
    zox_sys_on_end();
} zoxd_system(xr_render_system);

void zox_systems_xr_runtime(ecs* world) {
    // first, so the inputs (zoxp_inputs_extract) read the controllers at the frame's display time
    zox_system_1(xr_frame_begin_system, zoxp_begin, 0);
    zox_system_1(
        xr_controllers_system,
        zoxp_inputs_extract,
        [in] xr.XrHand,
        [out] inputs.DeviceHasInput,
        [none] xr.XrController
    );
    zox_system_1(xr_render_system, zoxp_render, 0);
}
