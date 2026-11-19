zox_tag(Gizmo);
zox_tag(GizmoOwner);
zox_tag(GizmoOwnerLink);  // a gizmo links to its owner, it is deleted with it
#include "raycast.c"
#include "character.c"
zoxc_custom(RaycastVoxelData);
zoxc_byte(RaycastType);
zoxc_float3(RaycastAim);    // a point its ray aims at, without a camera (an npc at its threat), not its facing

void zox_components_vrays(ecs *world) {
    zoxd_tag(Gizmo);
    zoxd_tag(GizmoOwner);
    zoxd_nf_link_owned(GizmoOwnerLink);
    zoxd(RaycastVoxelData);
    zoxd_float(RaycastRange);
    zoxd_byte(RaycastType);
    zoxd_float3(RaycastAim);
}
