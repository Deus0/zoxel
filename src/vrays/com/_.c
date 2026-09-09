zox_tag(Gizmo);
zox_tag(GizmoOwner);
#include "raycast.c"
#include "character.c"
zoxc_custom(RaycastVoxelData);
zoxc_byte(RaycastType);

void define_components_vrays(ecs *world) {
    zoxd_tag(Gizmo);
    zoxd_tag(GizmoOwner);
    zoxd(RaycastVoxelData);
    zoxd_float(RaycastRange);
    zoxd_byte(RaycastType);
}
