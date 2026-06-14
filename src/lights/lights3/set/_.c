// NOTE: These 'distances' can be very random, since propogation happens in many ways... so theyre more like algorithm limiters... - can remove if the rules are good to not break
#include "lights.c"
byte sunlight = 255;        // full sunlight
byte zox_debug_lights = 0;
byte darklight = 32;        // min light
// NOTE: Hello Darkness my old friend!
byte light_air_decay = 48; // 32;
byte light_propogation_distance = 8;
byte darklight_propogation_distance = 64;  // just keep going?
byte zox_is_log_lighting = 0;
byte zox_is_log_lighting_light = 0;
byte zox_is_log_lighting_remove = 0;
byte zox_is_log_lighting_dark = 0;
byte zox_is_log_lighting_place = 0;
byte zox_disable_low_res_lights = 0;
byte zox_disable_flood_fill = 0;
