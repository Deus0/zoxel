#include "sounds.c"

#include "pop.c"

void zox_systems_pickups(ecs* world) {
    zox_system_1(
        pickup_sound_system,
        zoxp_spawn,
        [in] PickedUp,
        [none] Pickup
    );
    zox_system(
        pickup_pop_system,
        zoxp_update,
        [in] PickupPopStart,
        [in] PickupPopEnd,
        [in] PickupPopHeight,
        [in] PickupPopTime,
        [in] PickedUp,
        [out] transforms3.Position3D,
        [none] PickupPop
    );
}
