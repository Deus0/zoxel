/*
 * +------------------------------------------------------+
 * | Zox Module: Inputs                                   |
 * |                                                      |
 * |  Keyboard - Mouse - Touch - Gamepad - State          |
 * |                                                      |
 * +------------------------------------------------------+
 *
 *      - defines base types for device handling
 *      - emulates state changes of physical devices
 *
 *  TODO:
 *
 *      - Refactor keyboard as a list of keys
 *      - If Device Disabled Detect Start/Connect inputs in another system
 *      - Use that state to connect a new device to a player
 *
 * */
const float bumper_button_cutoff = 0.98f;
const float joystick_min_cutoff = 0.08f;
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

void import_inputs(ecs* world) {
    zox_module(inputs);
    zox_module_dispose(module_dispose_inputs);
    initialize_hook_key_down();
    zox_components_inputs(world);
    zox_systems_input(world);
    add_hook_spawn_prefabs(spawn_prefabs_inputs);
}