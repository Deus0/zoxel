// TODO: Refactor chunk3 lights here
#ifndef zoxm_lights
#define zoxm_lights

zoxc_state(GenerateLights);
#include "lights3/_.c"

zox_begin_module(Lights) {
    zoxd_state(GenerateLights);
    zox_import_module(Lights3);
} zox_end_module(Lights);

#endif
