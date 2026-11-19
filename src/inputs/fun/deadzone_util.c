// a stick's value past a round dead zone: 0 inside it, outside scaled from 0 at its edge to 1 at the stick's, the way
//  kept (per axis cut offs made a square, a drifting stick's resting off the middle still moved)
static inline float2 stick_deadzone(float2 v, float deadzone) {
    const float length = sqrtf(v.x * v.x + v.y * v.y);
    if (length <= deadzone || deadzone >= 1) {
        return float2_zero;
    }
    const float scaled = fminf(1, (length - deadzone) / (1 - deadzone));
    return (float2) { v.x / length * scaled, v.y / length * scaled };
}

// #define zox_debug_deadzones

// dead zone magic
const float joystick_deadzone_time = 0.2f;
int joystick_deadzones_values[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int joystick_deadzones_new_values[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
double joystick_deadzone_times[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
byte joystick_deadzones_setting[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
byte joystick_deadzones_set[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int apply_joystick_deadzone(const int index, int axis_value) {
#if defined(zox_disable_gamepad_deadzones)
    return axis_value;
#endif
    if (zox_current_time < 0.1) return axis_value;
    // double current_time = joystick_deadzone_times[index]; // zox_current_time
    if (joystick_deadzones_set[index]) { // current_time >= joystick_deadzone_time) {
        axis_value = axis_value - joystick_deadzones_values[index];
        if (axis_value < -32768) axis_value = -32768;
        else if (axis_value > 32768) axis_value = 32768;
        return axis_value;
    } else if (!joystick_deadzones_setting[index]) {
        // can start deadzoning?
        joystick_deadzones_setting[index] = 1;
        joystick_deadzone_times[index] = zox_current_time;
        joystick_deadzones_new_values[index] = axis_value;
#ifdef zox_debug_deadzones
        zox_log(" > joystick probing possible deadzone [%i::%i]\n", index, axis_value)
#endif
    } else {
        // if deadzonining and value is still the same for x seconds, set new deadzon
        if (joystick_deadzones_new_values[index] == axis_value) {
            if (zox_current_time - joystick_deadzone_times[index] >= joystick_deadzone_time) {
                joystick_deadzones_set[index] = 1;
                joystick_deadzones_values[index] = axis_value;
#ifdef zox_debug_deadzones
                zox_log(" > joystick deadzone set [%i::%i]\n", index, axis_value)
#endif
            }
        } else {
            // start timing again
            joystick_deadzone_times[index] = zox_current_time;
            joystick_deadzones_new_values[index] = axis_value;
#ifdef zox_debug_deadzones
            zox_log(" > joystick reset deadzone [%i] set: %f\n", index, axis_value)
#endif
        }
    }
    return axis_value;
}
