// The navigator: LB and RB (a headset's grips) step through the taskbar's uis while paused, one open at a time
//  - the one the navigator's in (the one it last opened, else its selection's window, else the first open in the
//      row's order: ChildIndex) closes,
//      the one beside it opens, wrapping round; none open, RB opens the first, LB the last
//  - others stay open (a hud, i.e. the action bar, is a taskbar ui too); one beside it already open is moved into
//  - each through the taskbar's own toggle (taskbar_frame_toggle), so its frames light as a click lights them
//  - the new ui is the NavigationWindow and the navigators' selections are cleared: they select its first element
//      (element_navigation_begin_system), so a gamepad reaches every ui, not only the one it began in
//  - 2D and 3D taskbars alike (spawn_taskbar: spawn_window, 3D on 3D huds)
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

static entity navigator_opened = 0;     // the ui it last opened or moved into (the local player's)
static byte navigator_opened_kept = 0;  // it was open already (a hud: the action bar): left open when moved off

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
    entity devices[zox_children_capacity];
    const uint length = zox_get_children_by_id(world, player, devices, zox_children_capacity, zox_id(DeviceNavigator));
    // the window the selection's in
    entity selected_window = 0;
    for (uint i = 0; i < length && !selected_window; i++) {
        const entity target = zox_getv(devices[i], Target);
        selected_window = zox_valid(target) ? zox_get_parent_by_id(world, target, zox_id(Window)) : 0;
    }
    int current = -1;
    if (zox_valid(navigator_opened) && !zox_has(navigator_opened, UIClosing)) {
        for (int i = 0; i < count && current < 0; i++) {
            if (zox_getv(frames[i], ActiveState) && zox_has_id(navigator_opened, zox_getv(frames[i], TaskbarWindowID))) {
                current = i;
            }
        }
    }
    for (int i = 0; i < count && current < 0; i++) {
        if (zox_getv(frames[i], ActiveState) && zox_valid(selected_window) &&
            zox_has_id(selected_window, zox_getv(frames[i], TaskbarWindowID))) {
            current = i;
        }
    }
    for (int i = 0; i < count && current < 0; i++) {
        if (zox_getv(frames[i], ActiveState)) {
            current = i;
        }
    }
    const int next = current < 0 ? (direction > 0 ? 0 : count - 1) : (current + direction + count) % count;
    const byte current_kept = navigator_opened_kept && zox_valid(navigator_opened) && current >= 0 &&
        zox_has_id(navigator_opened, zox_getv(frames[current], TaskbarWindowID));
    if (current >= 0 && current != next && !current_kept) {
        taskbar_frame_toggle(world, frames[current], player);  // closed
    }
    entity window = 0;
    navigator_opened_kept = zox_getv(frames[next], ActiveState);
    if (navigator_opened_kept) {
        // open already: moved into, left open after
        window = zox_get_child_by_id(world, canvas, zox_getv(frames[next], TaskbarWindowID));
    } else {
        window = taskbar_frame_toggle(world, frames[next], player);
    }
    if (zox_valid(window)) {
        zox_add(window, NavigationWindow);
    }
    navigator_opened = window;
    // the navigators select within the new ui
    for (uint i = 0; i < length; i++) {
        raycaster_select_element(world, devices[i], 0);
    }
    return window;
}

// the player's game menus opened (Y, R): the row's first ui opens, the navigators' selections in it (a hud open
//  already, the action bar, isn't where it starts: stepping from it opened the one after it)
entity navigator_open_first(ecs* world, entity player) {
    const entity canvas = zox_get_link(world, player, CanvasLink);
    entity frames[navigator_frames_max];
    if (!zox_valid(canvas) || !navigator_frames(world, canvas, frames)) {
        return 0;
    }
    entity window = zox_getv(frames[0], ActiveState) ?
        zox_get_child_by_id(world, canvas, zox_getv(frames[0], TaskbarWindowID)) : taskbar_frame_toggle(world, frames[0], player);
    navigator_opened_kept = zox_getv(frames[0], ActiveState);
    if (zox_valid(window)) {
        zox_add(window, NavigationWindow);
    }
    navigator_opened = window;
    entity devices[zox_children_capacity];
    const uint length = zox_get_children_by_id(world, player, devices, zox_children_capacity, zox_id(DeviceNavigator));
    for (uint i = 0; i < length; i++) {
        raycaster_select_element(world, devices[i], 0);
    }
    return window;
}

// the first game menu opened, once the taskbar's frames are there and in order (spawned after it, ChildIndex), while
//  still in the menus
void delayed_open_game_menu(ecs* world, entity e) {
    if (!zox_valid(e) || zox_getv(e, PlayerState) != zox_player_state_paused) {
        return;
    }
    const entity canvas = zox_get_link(world, e, CanvasLink);
    entity frames[navigator_frames_max];
    if (!zox_valid(canvas) || !navigator_frames(world, canvas, frames)) {
        delay_event(world, &delayed_open_game_menu, e, 0.05f);
        return;
    }
    navigator_open_first(world, e);
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
