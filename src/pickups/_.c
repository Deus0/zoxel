#if !defined(zoxm_pickups) && defined(zoxm_cubes)
#define zoxm_pickups

// #define zox_prefabs_non_textured
#define pickup_rotaion_speed 40
zox_tag(Pickup);
zox_tag(PickUpperer);
zoxc_byte(PickedUp);
#include "dat/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "sys/_.c"
zox_increment_system(PickedUp, pickup_state_end);

zox_begin_module(Pickups)
    add_hook_spawn_prefabs(spawn_prefabs_pickups);
    zoxd_tag(Pickup);
    zoxd_tag(PickUpperer);
    zoxd_byte(PickedUp);
    zoxd_system_increment(PickedUp, [none] Pickup);
    zox_system_1(
        PickupSoundSystem,
        zoxp_mainthread,
        [in] PickedUp,
        [none] Pickup
    );
zox_end_module(Pickups)

#endif