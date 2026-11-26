#include "character.c"
#include "skeleton.c"

ushort prefab_characters_count = 0;
entity prefabs_characters3[32];
entity local_character3D;
entity prefab_character3;
entity prefab_character3_skeleton;

// setters for higher modules to add to characters
#define zox_prefab_character_set(T, ...) {\
    for (int i = 0; i < prefab_characters_count; i++) {\
        zox_set(prefabs_characters3[i], T, __VA_ARGS__)\
    }\
}

#define zox_prefab_character_addt(T) {\
    for (int i = 0; i < prefab_characters_count; i++) {\
        zox_add(prefabs_characters3[i], T);\
    }\
}

#define zox_prefab_character_add(T) {\
    for (int i = 0; i < prefab_characters_count; i++) {\
        zox_add(prefabs_characters3[i], T);\
    }\
}

// a voxel terrain's parts on every character prefab: game code adds them (gam/zoxel), as stats, a poly terrain's
//  characters don't have them (gam/zeland)
//  - in a chunk (LinkChunk, ChunkPosition, BlockPosition), the block it's in (InsideBlock..), raycasts into voxels,
//    pulled by the terrain's gravity field (src/terrains/voxel/gravity)
void prefab_characters3_add_voxels(ecs* world) {
    zox_prefab_character_add(LinkChunk);
    zox_prefab_character_set(ChunkPosition, { int3_chaos });
    zox_prefab_character_set(BlockPosition, { int3_zero });
    zox_prefab_character_set(InsideBlock, { 0 });
    zox_prefab_character_set(InsideBlockPosition, { int3_zero });
    zox_prefab_character_set(InsideBlockTime, { 0 });
    zox_prefab_character_set(InsideBlockLast, { 0 });
    zox_prefab_character_add(TerrainGravity);
    zox_prefab_character_set(RaycastVoxelData, { 0 });
    zox_prefab_character_set(RaycastType, { zox_raycast_type_select });
    zox_prefab_character_set(RaycastRange, { 1.5f });
}

void spawn_prefabs_characters3D(ecs *world) {
    prefab_character3 = spawn_prefab_character3(world);
    prefab_character3_skeleton = spawn_prefab_character3_skeleton(world, prefab_character3);
    prefabs_characters3[prefab_characters_count++] = prefab_character3;
    prefabs_characters3[prefab_characters_count++] = prefab_character3_skeleton;
    if (prefab_realm) {
        zox_set(prefab_realm, CharactersChanceMax, { 0 });
    }
}
