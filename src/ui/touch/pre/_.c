entity prefab_virtual_joystick;
#include "joystick.c"

void spawn_prefabs_touch(ecs* world) {
    prefab_virtual_joystick = spawn_prefab_virtual_joystick(world, prefab_element_frame);
}
