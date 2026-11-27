#define zox_disable_mouse_constraint
#define zox_disable_gamepad_stick_as_any_input // used for samsung phone, it's buggy af
#define zox_disable_gamepad_deadzones

byte disable_mouse_lock = 0;

// Fake mouse, for tests (see src/ui/tst/helpers.c)
//  - while zox_mouse_fake is set, the mouse device reads these instead of the os mouse
//  - position is in pointer space (bottom left origin), the same as ZevicePointerPosition
byte zox_mouse_fake = 0;
int2 zox_mouse_fake_position = { 0, 0 };
byte zox_mouse_fake_left = 0;
byte zox_mouse_fake_right = 0;
int2 zox_mouse_fake_wheel = { 0, 0 };

// Fake keyboard: tests read no real keyboard (they're automated, and a key pressed by whoever is at the machine would
//  change them): while set the os keyboard isn't read, its Keyboard keeps what a test sets (a held key stays held)
//  - on for every test (--tst, process_arguments_inputs)
byte zox_keyboard_fake = 0;

// Fake fingers, for tests (no touchscreen needed, see zox_tst_touch in src/ui/tst/helpers.c)
//  - while zox_touch_fake is set, the touchscreen reads these instead of sdl's fingers
//  - x and y are 0 to 1 across the window, top left origin (as sdl's fingers), id is the finger's (0 up)
#define zox_touch_fake_max 4
typedef struct {
    int id;
    float x;
    float y;
} zox_touch_finger;
byte zox_touch_fake = 0;
int zox_touch_fake_count = 0;
zox_touch_finger zox_touch_fake_fingers[zox_touch_fake_max];
