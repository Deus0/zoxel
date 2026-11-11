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
