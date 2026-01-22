#define zox_gamepad_layout_type_normal 0
#define zox_gamepad_layout_type_xbox 1
#define zox_gamepad_layout_type_steamdeck 2

#define zox_gamepad_button_count 14
#define zox_gamepad_stick_count 2
#define zox_gamepad_dpad_count 4

// These real indexes get mapped
/*#define zox_device_button_a 0
#define zox_device_button_b 1
#define zox_device_button_x 2
#define zox_device_button_y 3
#define zox_device_button_lb 4
#define zox_device_button_rb 5
#define zox_device_button_lt 6
#define zox_device_button_rt 7
#define zox_device_button_select 8
#define zox_device_button_start 9
#define zox_device_button_left_stick_push 10
#define zox_device_button_right_stick_push 11
#define zox_device_button_left_stick 12
#define zox_device_button_right_stick 13*/

static const byte generic_button_map[zox_gamepad_button_count] = {
    0,  // A
    1,  // B
    2,  // X
    3,  // Y
    4,  // LB
    5,  // RB
    6,  // LT
    7,  // RT
    9,  // Select
    10, // Start
    11, // LS Push
    12, // RS Push
    13, // ?
    8   // ?
};


// LB is 10?
// Start 6
// RSP is 8
static const byte steamdeck_button_map[zox_gamepad_button_count] = {
    0,  // A
    1,  // B
    2,  // X
    3,  // Y
    4,  // LB
    5,  // RB
    6,  // LT
    7,  // RT
    11, // Select
    10, // Start
    9,  // LS Push
    8,  // RS Push
    12,  // ?
    13, // ?
};

static const byte generic_stick_map[zox_gamepad_stick_count]   = { 0, 2 };
static const byte steamdeck_stick_map[zox_gamepad_stick_count] = { 0, 3 };

static const byte generic_dpad_map[zox_gamepad_dpad_count] = {
    zox_device_button_dpad_down,
    zox_device_button_dpad_up,
    zox_device_button_dpad_left,
    zox_device_button_dpad_right
};