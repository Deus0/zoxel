extern void hide_virtual_keyboard(SDL_Window*);

// NOTE: Used on FLX1S because phosh keeps putting it there!
zox_sys2(HideVirtualKeyboardSystem) {
    zox_sys_begin();
    zox_sys_in(SDLWindow);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(SDLWindow, window);
        if (is_virtual_keyboard_shown(window->value)) {
            // zox_logw("Virtual Keyboard shown");
            hide_virtual_keyboard(window->value);
        }
    }
} zox_sys_end(HideVirtualKeyboardSystem);


zox_sys2(ToggleVirtualKeyboardSystem) {
    zox_sys_world();
    if (!zox_valid(main_app) || !zox_has(main_app, SDLWindow)) {
        return;
    }
    const SDLWindow* window = zox_get(main_app, SDLWindow);
    zox_sys_begin();
    zox_sys_in(Keyboard);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Keyboard, keyboard);
        if (keyboard->left_control.is_pressed && keyboard->t.pressed_this_frame) {
            zox_log("Toggling Virtual Keyboard");
            toggle_virtual_keyboard(window->value);
        }
    }
} zox_sys_end(ToggleVirtualKeyboardSystem);