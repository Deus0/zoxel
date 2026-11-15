#include "settings.c"
#include "device_modes.c"
#include "gamepad_flags.c"
#include "button_flags.c"
#include "physical_button.c"
#include "physical_stick.c"
#include "maps.c"

#define zox_device_stick_left 0
#define zox_device_stick_right 1

// text typed this frame (sdl_events_system, SDL text input), a focused input field takes it (input_field_system)
char zox_text_typed[64];
// an input field is focused: the app turns text input on (SDL3 sends no text events without it)
byte zox_text_input_wanted = 0;

static inline void add_text_typed(const char* text) {
    strncat(zox_text_typed, text, sizeof(zox_text_typed) - strlen(zox_text_typed) - 1);
}
