#include "sounds.c"

void zox_define_systems_pickups(ecs* world) {
    zox_system_1(
        PickupSoundSystem,
        zoxp_mainthread,
        [in] PickedUp,
        [none] Pickup
    );
}
