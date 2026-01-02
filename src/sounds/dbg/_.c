// add debug keys to test sounds here
#include "toggle.c"

void zox_debug_sound(ecs* world, entity e) {
    zox_geter(e, SoundData, data);
    zox_log_error("Destroying Entity [%s]", zox_get_name(e));
    zox_log("   - sound [%i]", data->length);
}