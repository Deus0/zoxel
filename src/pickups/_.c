/*
 * +------------------------------------------------------+
 * | Zox Module: Pickups                                  |
 * |                                                      |
 * |  Loot - Drops - Collection - Spawning - State        |
 * |                                                      |
 * |  Uses: Physics - Geometry - Animations               |
 * |                                                      |
 * +------------------------------------------------------+
 */
// Uses: Geometry & Models
byte zox_disable_textured_items = 0;
float item_pickup_scale = 0.125f * 1.5f;
// #define zox_prefabs_non_textured
#define pickup_rotaion_speed 40
zox_tag(Pickup);
zox_tag(PickUpperer);
zoxc_state(PickedUp);
// Pop: a dropped item hops a little away from where it dropped (like borderlands)
zox_tag(PickupPop);
zoxc_float3(PickupPopStart);
zoxc_float3(PickupPopEnd);
zoxc_float(PickupPopHeight);
zoxc_double(PickupPopTime);
const double pickup_pop_seconds = 0.35;
#include "dat/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void import_pickups(ecs* world) {
    zox_module(pickups);
    zoxd_tag(Pickup);
    zoxd_tag(PickUpperer);
    zoxd_state(PickedUp);
    zoxd_tag(PickupPop);
    zoxd_float3(PickupPopStart);
    zoxd_float3(PickupPopEnd);
    zoxd_float(PickupPopHeight);
    zoxd_double(PickupPopTime);
    zox_systems_pickups(world);
    add_hook_spawn_prefabs(spawn_prefabs_pickups);
}
