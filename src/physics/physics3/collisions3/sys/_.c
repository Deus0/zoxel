static inline float get_distance_to_voxel_grid(float v, float direction, float scale) {
    float face = v;
    if (direction == 1) {
        face = floorf(v / scale) * scale;
    } else if (direction == 2) {
        face = ceilf(v / scale) * scale;
    }
    return absf(face - v);
}

#include "response.c"
#include "spheres.c"
#include "spheres_debug.c"
#include "friction.c"

#include "collision_debug.c"

void define_systems_collisions3(ecs *world) {
    // todo: reset collision on detection - so we can use it anywhere in our cycle
    //      maybe we keep last_collision state? and we response based on it?? idk
    //      idk how the systems might overlap so its hard
    zox_filter(
        sphere_colliders,
        [in] transforms3.Position3D,
        [in] SphereRadius,
        [in] physics.CollisionDisabled,
        [none] SphereCollider
    );
    zox_system_1(
        CollisionDebugSystem,
        zoxp_physics,
        [in] collisions3.CollisionDistance,
        [in] transforms3.Position3D,
        [in] collisions3.Collision,
        [in] transforms3.Bounds3D,
    );
    /*zox_system(
        CollisionResponseSystem,
        zoxp_physics,
        [in] collisions3.CollisionDistance,
        [out] transforms3.Position3D,
        [out] physics3.Velocity3D,
        [out] physics3.LastPosition3D,
        [out] collisions3.Collision,
        [out] collisions3.Grounded
    );*/
    zox_system(
        Friction3DSystem,
        zoxp_physics,
        [in] collisions3.Grounded,
        [out] physics3.Velocity3D,
        [none] physics.Frictioned
    );
    // TODO: split up between response and detect
    zox_system_ctx_1(
        SphereCollideSystem,
        zoxp_physics,
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
