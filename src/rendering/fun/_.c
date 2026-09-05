#include "mesh_indicies.c"
#include "resource_management.c"
#include "mesh.c"
#include "mesh_indicies2.c"
#include "shader_loading.c"
#include "render_passes.c"
#include "terminal.c"
#include "dispose.c"

// NOTE: Returns the render camera for a mesh (ui)
entity zox_get_mesh2_camera(ecs *world, entity e) {
    // This is render camera link basically
    entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
    if (!zox_valid(canvas)) {
        return 0;
    }
    entity camera = zox_get_link(world, canvas, Camera);
    return camera;
}
