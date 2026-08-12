entity prefab_virtual_joystick;
#include "joystick.c"

void spawn_prefabs_touch(ecs* world) {
    prefab_virtual_joystick = spawn_prefab_virtual_joystick(world, prefab_element_frame);
    if (prefab_finger) {
        zox_prefab_setv(prefab_finger, ElementLink, 0);
    }
}
