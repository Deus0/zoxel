#include "raycast.c"
#include "character.c"

void define_components_vrays(ecs *world) {
    zoxd(RaycastVoxelData);
    zox_define_component_float(RaycastRange);
}