// Temp system used by keyboard for clicking
// Basically a copy of DeviceClickSystem
zox_sys2(KeyboardClickSystem) {
    if (!keyboard_navigation_mode) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DeviceDisabled);
    zox_sys_in(PlayerLink);
    zox_sys_in(RaycasterTarget);
    zox_sys_in(WindowRaycasted);
    zox_sys_in(Keyboard);
    zox_sys_out(ClickingEntity);
    zox_sys_out(WindowTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(DeviceDisabled, disabled);
        zox_sys_i(PlayerLink, player);
        zox_sys_i(RaycasterTarget, target);
        zox_sys_i(WindowRaycasted, swindow);
        zox_sys_i(Keyboard, keyboard);
        zox_sys_o(ClickingEntity, clickee);
        zox_sys_o(WindowTarget, twindow);

        if (disabled->value) {
            continue;
        }

        if (!zox_valid(player->value) || !zox_has(player->value, CanvasLink)) {
            continue;
        }

        entity canvas = zox_get_value(player->value, CanvasLink);
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

        zox_geter_value(player->value, DeviceMode, byte, dmode);

        // released
        if (input_type == 1) {
            clickee->value = target->value;
            on_element_clicked(world, player->value, clickee->value);
        }

        // Initial Down
        if (input_type == 1) {

            if (swindow->value != twindow->value) {
                twindow->value = swindow->value;
                zox_set(canvas, WindowToTop, { twindow->value })
            }

            if (zox_valid(target->value) && zox_has(target->value, Dragable)) {
                byte drag_mode = zox_drag_mode_none;
                set_element_dragged(world, player->value, target->value, drag_mode);
            }

        }
        // Released
        else if (input_type == 2) {
            if (target->value == clickee->value) {
                on_element_released(world, player->value, target->value);
            }
            clickee->value = 0;
        }
    }
} zox_sys_end(KeyboardClickSystem);