#include "raycast.c"
#include "character.c"

void define_components_vrays(ecs *world) {
    zoxd(RaycastVoxelData);
    zoxd_float(RaycastRange);
}