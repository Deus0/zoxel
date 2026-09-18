#include "sounds.c"

void zox_systems_pickups(ecs* world) {
    zox_system_1(
        PickupSoundSystem,
        zoxp_spawn,
        [in] PickedUp,
        [none] Pickup
    );
}
