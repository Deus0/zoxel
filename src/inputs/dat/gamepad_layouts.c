#define zox_gamepad_layout_type_normal 0
#define zox_gamepad_layout_type_xbox 1
#define zox_gamepad_layout_type_steamdeck 2

#define zox_gamepad_button_count 14
#define zox_gamepad_stick_count 2
#define zox_gamepad_dpad_count 4

// These real indexes get mapped

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

/*static const byte steamdeck_button_map[zox_gamepad_button_count] = {
    0,  // A
    1,  // B
    2,  // X
    3,  // Y
    4,  // LB
    5,  // RB
    12, // SEL  (was 6)
    13, // STA  (was 7)
    10, // LS
    11, // RS
    8,  // LT
    9,  // RT
    6,  // ?    (was 12)
    7   // ?    (was 13)
};*/

/*static const byte steamdeck_button_map[zox_gamepad_button_count] = {
    0,  // A
    1,  // B
    2,  // X
    3,  // Y
    4,  // LB
    5,  // RB
    8,  // SEL (Select / Back)
    9,  // STA (Start / Menu)
    10, // LS  (Left stick press)
    11, // RS  (Right stick press)
    6,  // LT  (Left trigger press)
    7,  // RT  (Right trigger press)
    12, // ?
    13  // ?
};*/

static const byte steamdeck_button_map[zox_gamepad_button_count] = {
    0,  // A
    1,  // B
    2,  // X
    3,  // Y
    4,  // LB
    5,  // RB
    6,  // SEL (Select / Back)
    7,  // STA (Start / Menu)
    8, // LS  (Left stick press)
    9, // RS  (Right stick press)
    10,  // LT  (Left trigger press)
    11,  // RT  (Right trigger press)
    12, // ?
    13  // ?
};

static const byte generic_stick_map[zox_gamepad_stick_count]   = { 0, 2 };
static const byte steamdeck_stick_map[zox_gamepad_stick_count] = { 0, 3 };

static const byte generic_dpad_map[zox_gamepad_dpad_count] = {
    zox_device_button_dpad_down,
    zox_device_button_dpad_up,
    zox_device_button_dpad_left,
    zox_device_button_dpad_right
};