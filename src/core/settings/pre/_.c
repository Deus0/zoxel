#include "setting.c"

entity prefab_setting;

void spawn_prefabs_settings(ecs *world) {
    prefab_setting = spawn_prefab_setting(world);
}
