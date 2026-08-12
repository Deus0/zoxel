// resets the single frame states
byte reset_button_state(const byte previous_state) {
    byte new_state = previous_state;
    if (devices_get_pressed_this_frame(previous_state)) {
        devices_set_pressed_this_frame(&new_state, 0);
    } else if (devices_get_released_this_frame(previous_state))  {
        devices_set_released_this_frame(&new_state, 0);
    }
    return new_state;
}