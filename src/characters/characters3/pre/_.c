#include "character3_meta.c"
#include "character3.c"
#include "character3_instanced.c"
#include "character3_skeleton.c"

ushort prefab_characters_count = 0;
entity prefabs_characters3[32];
entity local_character3D;
entity prefab_character3_meta;
entity prefab_character3;
entity prefab_character3_instanced;
entity prefab_character3_skeleton;

// setters for higher modules to add to characters
#define zox_prefab_character_set(T, ...) {\
    for (int i = 0; i < prefab_characters_count; i++) {\
        zox_prefab_set(prefabs_characters3[i], T, __VA_ARGS__)\
    }\
}

#define zox_prefab_character_add(T) {\
    for (int i = 0; i < prefab_characters_count; i++) {\
        zox_prefab_add(prefabs_characters3[i], T)\
    }\
}

void spawn_prefabs_characters3D(ecs *world) {
    prefab_character3_meta = spawn_prefab_character3_meta(world);

    prefab_character3 = spawn_prefab_character3(world, prefab_vox, zox_character_type_unique);
    prefab_character3_instanced = spawn_prefab_character3_instanced(world, prefab_vox_instanced);
    prefab_character3_skeleton = spawn_prefab_character3_skeleton(world, prefab_vox);

    prefabs_characters3[prefab_characters_count++] = prefab_character3;
    prefabs_characters3[prefab_characters_count++] = prefab_character3_instanced;
    prefabs_characters3[prefab_characters_count++] = prefab_character3_skeleton;

    if (prefab_realm) {
        zox_prefab_add(prefab_realm, CharacterLinks);
        zox_prefab_set(prefab_realm, CharactersChanceMax, { 0 });
    }
}
