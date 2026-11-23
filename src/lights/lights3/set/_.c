// NOTE: These 'distances' can be very random, since propogation happens in many ways... so theyre more like algorithm limiters... - can remove if the rules are good to not break
byte zox_no_lights = 0;
// #include "lights.c"
byte sunlight = 255;        // full sunlight
byte zox_debug_lights = 0;
byte darklight = 16;        // min light
// NOTE: Hello Darkness my old friend!
byte light_air_decay = 32; // 32;
byte light_water_decay = 24;    // a sunbeam's light lost in each liquid block it passes (doc/tech/water.md)
byte light_propogation_distance = 12;
byte darklight_propogation_distance = 64;  // just keep going?
byte zox_is_log_lighting = 0;
byte zox_is_log_lighting_light = 0;
byte zox_is_log_lighting_remove = 0;
byte zox_is_log_lighting_dark = 0;
byte zox_is_log_lighting_place = 0;
byte zox_disable_low_res_lights = 0;
byte zox_disable_flood_fill = 0;

// a worker thread's time a frame flooding lights and casting sunbeams (light_flood_system, light_beam_system): was 256
//  of each a call (~25us), chunks a lod swap relit held thousands queued for hundreds of frames (their shown meshes held,
//  their swaps waiting) and far lods waited long for sunlight, drawn dark then lit (tst chunk_mesh_flicker)
#define zox_light_flood_budget_ms 3.0
