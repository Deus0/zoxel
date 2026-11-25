#include "character.c"
entity prefab_character3_npc;
entity prefab_character3_skeleton_npc;


void spawn_prefabs_npcs(ecs *world) {
    prefab_character3_npc = spawn_prefab_character3_npc(
        world,
        prefab_character3);
    prefab_character3_skeleton_npc = spawn_prefab_character3_npc(
        world,
        prefab_character3_skeleton);
    // add to character list
    prefabs_characters3[prefab_characters_count++] = prefab_character3_npc;
    prefabs_characters3[prefab_characters_count++] = prefab_character3_skeleton_npc;
}
