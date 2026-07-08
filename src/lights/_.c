// TODO: Refactor chunk3 lights here
#ifndef zoxm_lights
#define zoxm_lights

#define zox_generate_lights_start 1
#define zox_generate_lights_sunlight 1
#define zox_refresh_lights 1
byte zox_smooth_lighting = 1;
zoxc_byte(GenerateLights);
#include "lights3/_.c"

zox_begin_module(Lights) {
    zoxd_byte(GenerateLights);
    zox_import_module(Lights3);
} zox_end_module(Lights);

#endif
