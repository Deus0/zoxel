// add debug keys to test sounds here
#include "toggle.c"

void zox_debug_sound(ecs* world, entity e) {
    const SoundData* data = zox_get(e, SoundData);
    zox_loge("Destroying Entity [%s]", zox_get_name(e));
    zox_log("   - sound [%i]", data->length);
}