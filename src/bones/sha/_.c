#include "bone.c"

entity material_bone;
entity material_bonet;

void spawn_shaders_bones(ecs *world) {
    material_bone = spawn_material_bone(world, 0);
    material_bonet = spawn_material_bone(world, 1);
}
