// The navigator: LB and RB (a headset's grips) step through the taskbar's uis while paused, one open at a time
//  - the open one (the first frame active, in the row's order: ChildIndex) closes, the one beside it opens,
//      wrapping round; none open, RB opens the first, LB the last
//  - each through the taskbar's own toggle (taskbar_frame_toggle), so its frames light as a click lights them
//  - the new ui is the NavigationWindow and the navigators' selections are cleared: they select its first element
//      (element_navigation_begin_system), so a gamepad reaches every ui, not only the one it began in
//  - 2D and 3D taskbars alike (spawn_taskbar, spawn_taskbar3)
#define navigator_frames_max 32

// the taskbar's frames in the row's order
static int navigator_frames(ecs* world, entity canvas, entity* frames) {
    const entity taskbar = zox_get_child_by_id(world, canvas, zox_id(Taskbar));
    const entity body = zox_valid(taskbar) ? zox_get_child_by_id(world, taskbar, zox_id(WindowBody)) : 0;
    if (!zox_valid(body)) {
        return 0;
    }
    int count = 0;
    iter it = zox_children(world, body);
    while (zox_children_next(it)) {
        for (int j = 0; j < it.count; j++) {
            const entity e2 = it.entities[j];
            if (count < navigator_frames_max && zox_has(e2, TaskbarWindowID) && zox_has(e2, ChildIndex)) {
                frames[count++] = e2;
            }
        }
    }
    for (int a = 1; a < count; a++) {
        const entity e2 = frames[a];
        const uint index = zox_getv(e2, ChildIndex);
        int b = a;
        while (b > 0 && zox_getv(frames[b - 1], ChildIndex) > index) {
            frames[b] = frames[b - 1];
            b--;
        }
        frames[b] = e2;
    }
    return count;
}

// steps the player's taskbar uis by direction (1 RB, -1 LB), returns the ui opened
entity navigator_shift(ecs* world, entity player, int direction) {
    const entity canvas = zox_get_link(world, player, CanvasLink);
    if (!zox_valid(canvas)) {
        return 0;
    }
    entity frames[navigator_frames_max];
    const int count = navigator_frames(world, canvas, frames);
    if (!count) {
        return 0;
    }
    int current = -1;
    for (int i = 0; i < count; i++) {
        if (zox_getv(frames[i], ActiveState)) {
            if (current < 0) {
                current = i;
            }
            taskbar_frame_toggle(world, frames[i], player);     // closed, one open at a time
        }
    }
    const int next = current < 0 ? (direction > 0 ? 0 : count - 1) : (current + direction + count) % count;
    const entity window = taskbar_frame_toggle(world, frames[next], player);
    if (zox_valid(window)) {
        zox_add(window, NavigationWindow);
    }
    // the navigators select within the new ui
    entity devices[zox_children_capacity];
    const uint length = zox_get_children_by_id(world, player, devices, zox_children_capacity, zox_id(DeviceNavigator));
    for (uint i = 0; i < length; i++) {
        raycaster_select_element(world, devices[i], 0);
    }
    return window;
}

// LB or RB pressed this frame on any of the player's devices
void navigator_shift_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        if (state->value != zox_player_state_paused) {
            continue;
        }
        int direction = 0;
        entity devices[zox_children_capacity];
        const uint length = zox_get_children_by_id(world, e, devices, zox_children_capacity, zox_id(Device));
        for (uint j = 0; j < length; j++) {
            if (!zox_valid(devices[j]) || zox_has(devices[j], Disabled)) {
                continue;
            }
            entity zevices[zox_children_capacity];
            const uint zevices_length = zox_get_children(world, devices[j], zevices, zox_children_capacity);
            for (uint k = 0; k < zevices_length; k++) {
                const entity e3 = zevices[k];
                if (zox_has(e3, Disabled) || !zox_has(e3, ZeviceButton) || !devices_get_pressed_this_frame(zox_getv(e3, ZeviceButton))) {
                    continue;
                }
                const byte type = zox_getv(e3, DeviceButtonType);
                if (type == zox_btn_lb) {
                    direction--;
                } else if (type == zox_btn_rb) {
                    direction++;
                }
            }
        }
        if (direction) {
            navigator_shift(world, e, direction > 0 ? 1 : -1);
        }
    }
    zox_sys_on_end();
} zoxd_system(navigator_shift_system);
