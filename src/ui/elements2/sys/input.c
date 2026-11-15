// the focused input field takes the keyboard's typing (zox_text_typed, cleared here), backspace, and enter or escape
//  to unfocus
void input_field_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Keyboard);
    if (input_field_focus && !zox_valid(input_field_focus)) {
        input_field_focus = 0;
        zox_text_input_wanted = 0;
    }
    for (int i = 0; i < it->count && input_field_focus; i++) {
        zox_sys_i(Keyboard, keyboard);
        input_field_type(world, zox_text_typed);
        if (keyboard->backspace.pressed_this_frame) {
            input_field_backspace(world);
        }
        if (keyboard->enter.pressed_this_frame || keyboard->escape.pressed_this_frame) {
            input_field_unfocus(world);
        }
    }
    // taken, or typed with nothing focused
    zox_text_typed[0] = 0;
    zox_sys_on_end();
} zoxd_system(input_field_system);
