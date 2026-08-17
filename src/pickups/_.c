/*
 * +------------------------------------------------------------------+
 * | Zox Module: Pickups                                              |
 * |                                                                  |
 * |  Loot - Drops - Collection - Spawning - Pickup State             |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#if !defined(zoxm_pickups) && defined(zoxm_cubes)
#define zoxm_pickups

byte zox_disable_textured_items = 0;
float item_pickup_scale = 0.125f * 1.5f;
// #define zox_prefabs_non_textured
#define pickup_rotaion_speed 40
zox_tag(Pickup);
zox_tag(PickUpperer);
zoxc_state(PickedUp);
#include "dat/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Pickups) {
    zoxd_tag(Pickup);
    zoxd_tag(PickUpperer);
    zoxd_state(PickedUp);
    zox_define_systems_pickups(world);
    add_hook_spawn_prefabs(spawn_prefabs_pickups);
} zox_end_module(Pickups);

#endif
