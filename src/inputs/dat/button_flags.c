// button flags
#define zox_device_is_pressed 1 // 00000001
#define zox_device_pressed_this_frame 2 // 00000010
#define zox_device_released_this_frame 3 // 00000100
#define zox_device_pressed_last_frame 4

// wrappers
static inline void devices_set_is_pressed(byte *value, byte state) {
    set_bit(value, zox_device_is_pressed, state);
}

static inline byte devices_get_pressed(byte value) {
    return get_bit(value, zox_device_is_pressed);
}

static inline void devices_set_pressed_this_frame(byte *value, byte state) {
    set_bit(value, zox_device_pressed_this_frame, state);
}

static inline byte devices_get_pressed_this_frame(byte value) {
    return get_bit(value, zox_device_pressed_this_frame);
}

static inline void devices_set_released_this_frame(byte *value, byte state) {
    set_bit(value, zox_device_released_this_frame, state);
}

static inline byte devices_get_released_this_frame(byte value) {
    return get_bit(value, zox_device_released_this_frame);
}


static inline void devices_set_pressed_last_frame(byte *value, byte state) {
    set_bit(value, zox_device_pressed_last_frame, state);
}

static inline byte devices_get_pressed_last_frame(byte value) {
    return get_bit(value, zox_device_pressed_last_frame);
}