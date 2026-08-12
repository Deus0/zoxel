#include "response.c"
#include "spheres.c"
#include "spheres_debug.c"
#include "friction.c"
#include "collision_debug.c"

void define_systems_collisions3(ecs *world) {
    // todo: reset collision on detection - so we can use it anywhere in our cycle
    //      maybe we keep last_collision state? and we response based on it?? idk
    //      idk how the systems might overlap so its hard
    zox_system_1(
        CollisionDebugSystem,
        zoxp_mainthread,
        [in] collisions3.CollisionDistance,
        [in] transforms3.Position3D,
        [in] collisions3.Collision,
        [in] transforms3.Bounds3D,
    );
    // TODO: split up between response and detect
    zox_filter(
        sphere_colliders,
        [in] transforms3.Position3D,
        [in] SphereRadius,
        [in] physics.CollisionDisabled,
        [none] SphereCollider
    );
    zox_system_ctx_1(
        SphereCollideSystem,
        zoxp_mainthread,
        sphere_colliders,
        [in] transforms3.Position3D,
        [in] SphereRadius,
        [in] physics.CollisionDisabled,
        [none] SphereCollider
    );
#ifdef zox_gizmos_sphere_colliders
    zox_system_1(
        SphereColliderDrawSystem,
        zoxp_mainthread,
        [in] rendering.RenderDisabled,
        [in] transforms3.Position3D,
        [in] SphereRadius,
        [none] SphereCollider
    );
#endif
}
