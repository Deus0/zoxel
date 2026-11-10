#include "bone.c"
#include "skeleton_mesh.c"
#include "skeleton.c"
entity prefab_bone;
entity prefab_skeleton;
entity prefab_skeleton_mesh;

void spawn_prefabs_bones(ecs* world) {
    prefab_bone = spawn_prefab_bone(world);
    prefab_skeleton = spawn_prefab_skeleton(world);
    prefab_skeleton_mesh = spawn_prefab_skeleton_mesh(world);
}
