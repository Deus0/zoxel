#include "mesh_indicies.c"
#include "resource_management.c"
#include "mesh.c"
#include "mesh_indicies2.c"
#include "shader_loading.c"
#include "render_passes.c"
#include "terminal.c"
#include "dispose.c"

entity zox_get_root_canvas_camera(ecs *world, entity e) {
    entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
    return (zox_valid(canvas) && zox_has(canvas, CameraLink)) ? zox_gett_value(canvas, CameraLink) : 0;
}
