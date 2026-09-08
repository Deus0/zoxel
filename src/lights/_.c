// TODO: Refactor chunk3 lights here
#define zox_generate_lights_sunlight 1
#define zox_refresh_lights 1
byte zox_smooth_lighting = 1;
#include "com/_.c"
#include "fun/_.c"
#include "lights3/_.c"

void import_lights(ecs* world) {
    zox_module(lights);
    zoxd_components_lights(world);
    zox_import_module(Lights3);
}
