#include "character.c"
entity prefab_character3_npc;
entity prefab_character3_instanced_npc;
entity prefab_character3_skeleton_npc;

#define zox_prefab_character_npc_set(T, ...)\
    zox_prefab_set(prefab_character3_npc, T, __VA_ARGS__)\
    zox_prefab_set(prefab_character3_instanced, T, __VA_ARG)

void spawn_prefabs_npcs(ecs *world) {
    prefab_character3_npc = spawn_prefab_character3_npc(world, prefab_character3);
    prefab_character3_instanced_npc = spawn_prefab_character3_npc(world, prefab_character3_instanced);
    {
        // Disabled on NPCs for now
        prefab_character3_skeleton_npc = spawn_prefab_character3_npc(world, prefab_character3_skeleton);
        zox_add_tag(prefab_character3_skeleton_npc, NoSplitChunk);
    }
    // add to character list
    prefabs_characters3[prefab_characters_count++] = prefab_character3_npc;
    prefabs_characters3[prefab_characters_count++] = prefab_character3_instanced_npc;
    prefabs_characters3[prefab_characters_count++] = prefab_character3_skeleton_npc;
}
