// Temp system used by keyboard for clicking
// Basically a copy of DeviceClickSystem
zox_sys2(KeyboardClickSystem) {
    if (!keyboard_navigation_mode) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceDisabled);
    zox_sys_in(EntityTarget);
    zox_sys_in(Keyboard);
    zox_sys_out(ClickingEntity);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DeviceDisabled, disabled);
        zox_sys_i(EntityTarget, target);
        zox_sys_i(Keyboard, keyboard);
        zox_sys_o(ClickingEntity, clickee);
        if (disabled->value) {
            continue;
        }
        entity player = zox_get_link(world, e, PlayerLink);
        if (!zox_valid(player)) {
            continue;
        }
        entity canvas = zox_get_link(world, player, CanvasLink);
        if (!zox_valid(canvas)) {
            continue;
        }
        byte input_type = 0;
        if (keyboard->enter.pressed_this_frame) {
            input_type = 1;
        } else if (keyboard->enter.released_this_frame) {
            input_type = 2;
        }
        if (keyboard->escape.pressed_this_frame || keyboard->backspace.pressed_this_frame) {
            zox_log("TODO: Click the close button from here");
        }
        if (!input_type) {
            continue;
        }
        // released
        if (input_type == 1) {
            clickee->value = target->value;
            on_element_clicked(
                world,
                player,
                clickee->value);
        }
        // Initial Down
        if (input_type == 1) {
            /*if (swindow->value != twindow->value) {
                twindow->value = swindow->value;
                zox_set(canvas, WindowToTop, { twindow->value })
            }*/

            if (zox_valid(target->value) && zox_has(target->value, Dragable)) {
                byte drag_mode = zox_drag_mode_none;
                set_element_dragged(
                    world,
                    player,
                    target->value,
                    drag_mode);
            }

        }
        // Released
        else if (input_type == 2) {
            if (target->value == clickee->value) {
                on_element_released(
                    world,
                    player,
                    target->value);
            }
            clickee->value = 0;
        }
    }
} zox_sys_end(KeyboardClickSystem);
