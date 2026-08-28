/*
 * +------------------------------------------------------------------+
 * | Zox Module: Weathers                                             |
 * |                                                                  |
 * |  Skybox - Clouds - Rain - Wind - Fog                             |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
byte override_sky = 0;
color_rgb override_sky_fill = color_rgb_red;
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"

void import_weathers(ecs* world) {
    zox_module(weathers);
    zox_components_weather(world);
    zox_systems_weather(world);
    add_hook_spawn_prefabs(zox_prefabs_weather);
    add_hook_spawn_prefabs(zox_events_weather);
    add_hook_load_shader(&zox_shaders_weather);
}
