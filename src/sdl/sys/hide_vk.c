extern void hide_virtual_keyboard();

// NOTE: Used on FLX1S because phosh keeps putting it there!
void HideVirtualKeyboardSystem(iter *it) {
    zox_sys_begin();
    zox_sys_in(SDLWindow);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(SDLWindow, window);
        if (is_virtual_keyboard_shown(window->value)) {
            // zox_logw("Virtual Keyboard shown");
            hide_virtual_keyboard();
        }
    }
} zoxd_system2(HideVirtualKeyboardSystem);


void ToggleVirtualKeyboardSystem(iter *it) {
    zox_sys_begin();
    zox_sys_in(Keyboard);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Keyboard, keyboard);
        if (keyboard->left_control.is_pressed && keyboard->t.pressed_this_frame) {
            zox_log("Toggling Virtual Keyboard");
            toggle_virtual_keyboard();
        }
    }
} zoxd_system2(ToggleVirtualKeyboardSystem);