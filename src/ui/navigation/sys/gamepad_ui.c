// a gamepad's ui beyond stepping the selection (element_navigation_system), the same in 2D and 3D (XR's controllers
//  are a gamepad): a selected slider stepped by the stick's left and right, a tab window's tabs by LB and RB
#define navigation_slider_steps 20      // a float slider's steps end to end (a byte one steps by 1, up to this many)
#define navigation_slider_repeat 0.25   // seconds between steps, the stick held

// the stick's x across a navigator's zevices, right positive (a stick's x as SDL has it, right is -x)
static inline float navigator_stick_right(ecs* world, entity navigator) {
    float x = 0;
    entity zevices[zox_children_capacity];
    const uint count = zox_get_children(world, navigator, zevices, zox_children_capacity);
    for (uint k = 0; k < count; k++) {
        if (zox_has(zevices[k], ZeviceStick) && !zox_has(zevices[k], Disabled)) {
            x += zox_getv(zevices[k], ZeviceStick).x;
        }
    }
    return -x;
}

// a button of type pressed this frame on any of the player's enabled devices (XR: LB's the left hand's, RB the right's)
static inline byte player_button_pressed(ecs* world, entity player, byte type) {
    iter it = zox_children(world, player);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            const entity device = it.entities[i];
            if (!zox_has(device, Device) || zox_has(device, Disabled)) {
                continue;
            }
            iter it2 = zox_children(world, device);
            while (zox_children_next(it2)) {
                for (int j = 0; j < it2.count; j++) {
                    const entity e = it2.entities[j];
                    if (zox_has(e, ZeviceButton) && zox_has(e, DeviceButtonType) && zox_getv(e, DeviceButtonType) == type &&
                        devices_get_pressed_this_frame(zox_getv(e, ZeviceButton))) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

static inline byte navigator_in_gamepad_mode(ecs* world, entity navigator) {
    const entity player = zox_get_parent(world, navigator);
    return zox_valid(player) && zox_has(player, DeviceMode) && zox_getv(player, DeviceMode) == zox_device_mode_gamepad;
}

// steps a slider's handle a step along (direction -1 or 1), its SlideEvent told the value as a drag tells it
static inline void navigation_slider_step(ecs* world, entity player, entity slider, int direction) {
    entity handle = 0;
    iter it = zox_children(world, slider);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count && !handle; i++) {
            if (zox_has(it.entities[i], SlideBounds)) {
                handle = it.entities[i];
            }
        }
    }
    if (!zox_valid(handle) || !zox_has(handle, LayoutConstraints)) {
        return;
    }
    const float2 bounds = zox_getv(handle, SlideBounds);
    const int4 limits = zox_getv(handle, LayoutConstraints);
    const int width = limits.y - limits.x;
    const float range = bounds.y - bounds.x;
    if (width <= 0 || range <= 0) {
        return;
    }
    // a whole numbered range of 2 to navigation_slider_steps steps by 1 (a byte slider's), else in steps (0 to 1 too)
    const float step = (range >= 2 && range <= navigation_slider_steps && range == (int) range) ? 1 :
        range / navigation_slider_steps;
    const int2 position = zox_getv(handle, LayoutPosition);
    const float now = bounds.x + range * (position.x - limits.x) / (float) width;
    float value = now + direction * step;
    value = value < bounds.x ? bounds.x : (value > bounds.y ? bounds.y : value);
    if (step == 1) {
        value = roundf(value);
    }
    const int x = limits.x + (int) roundf((value - bounds.x) / range * width);
    zox_setv(handle, LayoutPosition, ((int2) { x, position.y }));
    if (zox_has(handle, SlideEvent) && zox_getv(handle, SlideEvent)) {
        const SlideEventData data = { .dragged = handle, .player = player, .value = value };
        (*zox_getv(handle, SlideEvent))(world, &data);
    }
}

// a selected slider (or its handle) stepped by its navigator's stick left and right: a step a push, again every
//  navigation_slider_repeat held
void navigation_slider_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Target);
    static entity held_by[8];
    static double held_next[8];
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Target, target);
        int slot = -1;
        for (int k = 0; k < 8 && slot < 0; k++) {
            slot = held_by[k] == e ? k : slot;
        }
        for (int k = 0; k < 8 && slot < 0; k++) {
            if (!held_by[k] || !zox_valid(held_by[k])) {
                held_by[k] = e;
                held_next[k] = 0;
                slot = k;
            }
        }
        entity slider = target->value;
        if (zox_valid(slider) && zox_has(slider, SlideBounds)) {
            slider = zox_get_parent(world, slider);
        }
        if (slot < 0 || !zox_valid(slider) || !zox_has(slider, SliderLabel) || !navigator_in_gamepad_mode(world, e)) {
            continue;
        }
        const float right = navigator_stick_right(world, e);
        if (float_abs(right) <= ui_navigation_joystick_cutoff) {
            held_next[slot] = 0;
            continue;
        }
        if (held_next[slot] && zox_current_time < held_next[slot]) {
            continue;
        }
        held_next[slot] = zox_current_time + navigation_slider_repeat;
        navigation_slider_step(world, zox_get_parent(world, e), slider, right > 0 ? 1 : -1);
    }
    zox_sys_on_end();
} zoxd_system(navigation_slider_system);

// LB and RB pick the tab before and after the shown one in the selection's tab window (wrapping), the selection moved
//  to the new tab's button (the old tab's elements hide)
void navigation_tabs_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Target);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Target, target);
        if (!zox_valid(target->value) || !navigator_in_gamepad_mode(world, e)) {
            continue;
        }
        const entity player = zox_get_parent(world, e);
        const int direction = player_button_pressed(world, player, zox_btn_rb) ? 1 :
            (player_button_pressed(world, player, zox_btn_lb) ? -1 : 0);
        if (!direction) {
            continue;
        }
        const entity window = zox_get_parent_by_id(world, target->value, zox_id(Window));
        if (!zox_valid(window)) {
            continue;
        }
        entity tabs[16];
        const uint count = zox_get_children_by_id_recursive(world, window, tabs, 16, zox_id(TabButton), 0);
        if (count < 2) {
            continue;
        }
        int active = 0;
        for (uint k = 0; k < count; k++) {
            if (zox_getv(tabs[k], ActiveState)) {
                active = k;
            }
        }
        const entity next = tabs[(active + direction + (int) count) % (int) count];
        on_click_window_tab(world, (ClickEventData) { .clicker = player, .clicked = next });
        raycaster_select_element(world, e, next);
    }
    zox_sys_on_end();
} zoxd_system(navigation_tabs_system);
