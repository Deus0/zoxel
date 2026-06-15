#define zox_gamepad_layout_type_normal 0
#define zox_gamepad_layout_type_xbox 1
#define zox_gamepad_layout_type_steamdeck 2

#define zox_gamepad_button_count 14
#define zox_gamepad_stick_count 2
#define zox_gamepad_dpad_count 4

static const byte gamepad_map_8bitdo[zox_gamepad_button_count] = {
    0,  // A
    1,  // B
    2,  // X
    3,  // Y
    9,  // LB
    10, // RB
    8,  // LT
    9,  // RT
    4, // Select
    6, // Start
    13, // LS Push
    14, // RS Push
    12, // Logo Button
    8   // ?
};
static const byte generic_stick_map[zox_gamepad_stick_count] = { 0, 2 };


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
static const byte steamdeck_stick_map[zox_gamepad_stick_count] = { 0, 3 };

static const byte generic_dpad_map[zox_gamepad_dpad_count] = {
    zox_btn_dpad_down,
    zox_btn_dpad_up,
    zox_btn_dpad_left,
    zox_btn_dpad_right
};
