// TODO: Refactor chunk3 lights here
#define zox_generate_lights_sunlight 1
#define zox_refresh_lights 1
byte zox_smooth_lighting = 1;
#include "com/_.c"
#include "fun/_.c"
#include "lights3/_.c"
#include "eve/_.c"

// --noemitters: glow stones and lamps don't light, to measure what their light costs (doc/plans/day_night.md)
void process_arguments_lights(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "--noemitters") == 0) {
            zox_no_emitters = 1;
        }
    }
}
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_lights(ecs* world) {
    zox_module(lights);
    add_hook_spawn_prefabs(zox_events_lights);
    add_hook_terminal_command(process_arguments_lights);
    zoxd_components_lights(world);
    zox_add_module(lights3);
#ifdef zox_tests
    zox_tsts_lights();
    zox_tsts_lights_gpu(world);
    zox_tsts_lights_gpu_neighbours(world);
    zox_tsts_lights_gpu_stress(world);
#endif
}
