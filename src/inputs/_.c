/*
 *  Zox Input
 *
 *      - defines base types for device handling
 *      - emulates state changes of physical devices
 *
 *  todo:
 *
 *      - refactor keyboard as a list of keys
 *
 * */
#ifndef zoxm_inputs
#define zoxm_inputs

const float bumper_button_cutoff = 0.98f;
const float joystick_min_cutoff = 0.02f;
// TODO: If Device Disabled, just detect Start/Connect inputs in another system
// - Use that state to connect a new device to a player

#include "set/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

void module_dispose_inputs(ecs *world, void *ctx) {
    (void) world;
    (void) ctx;
    dispose_hook_key_down();
}

void add_hook_key_down_2(int keycode, void *e) {

}

zox_begin_module(Inputs) {
    zox_module_dispose(module_dispose_inputs);
    initialize_hook_key_down();
    define_components_inputs(world);
    define_systems_input(world);
    add_hook_spawn_prefabs(spawn_prefabs_inputs);
} zox_end_module(Inputs);

#endif
