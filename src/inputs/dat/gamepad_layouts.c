#define zox_gamepad_layout_type_normal 0
#define zox_gamepad_layout_type_xbox 1
#define zox_gamepad_layout_type_steamdeck 2

#define zox_gamepad_button_count 14
#define zox_gamepad_stick_count 2
#define zox_gamepad_dpad_count 4

static const byte generic_button_map[zox_gamepad_button_count] = {
    /* A  */ 0,
    /* B  */ 1,
    /* X  */ 2,
    /* Y  */ 3,
    /* LB */ 4,
    /* RB */ 5,
    /* SEL*/ 6,
    /* STA*/ 7,
    /* LS */ 9,
    /* RS */ 10,
    /* LT */ 11,
    /* RT */ 12,
    /* ?  */ 12,
    /* ?  */ 13
};

static const byte steamdeck_button_map[zox_gamepad_button_count] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    10, 11, 8, 9, 12, 13
};

static const byte generic_stick_map[zox_gamepad_stick_count]   = { 0, 2 };
static const byte steamdeck_stick_map[zox_gamepad_stick_count] = { 0, 3 };

static const byte generic_dpad_map[zox_gamepad_dpad_count] = {
    zox_device_button_dpad_down,
    zox_device_button_dpad_up,
    zox_device_button_dpad_left,
    zox_device_button_dpad_right
};