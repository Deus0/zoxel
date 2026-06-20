zoxc_entity(GizmoLink);
#include "raycast.c"
#include "character.c"
zoxc_custom(RaycastVoxelData);
zoxc_byte(RaycastType);

void define_components_vrays(ecs *world) {
    zoxd(RaycastVoxelData);
    zoxd_float(RaycastRange);
    zoxd_entity(GizmoLink);
    zoxd_byte(RaycastType);
}
