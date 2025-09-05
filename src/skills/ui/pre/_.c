entity prefab_menu_skills;
entity prefab_icon_skill;
entity prefab_frame_skill;
#include "icon.c"
#include "frame.c"
#include "menu.c"

void spawn_prefabs_ui_skills(ecs *world) {
    prefab_icon_skill = spawn_prefab_icon_skill(world, prefab_icon_user);
    prefab_frame_skill = spawn_prefab_frame_skill(world, prefab_frame_user);
    prefab_menu_skills = spawn_prefab_menu_skills(world, prefab_window_users);
    zox_set(prefab_menu_skills, FramePrefabLink, { prefab_frame_skill });
}