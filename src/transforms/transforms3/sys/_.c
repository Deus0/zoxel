#include "euler_override.c"
#include "euler_limit_x.c"
#include "euler_limit_z.c"
#include "transform_matrix.c"
#include "transform_matrix_scale.c"
#include "camera_transform_matrix.c"
#include "shadow.c"
#include "children.c"

void transform_sync_system(iter* it) { }
zoxd_system(transform_sync_system);

void zox_define_systems_transforms3(ecs *world) {
    zox_system_1(
        transform_sync_system,
        zoxp_transform_sync
    );
    zox_system(
        transform3_children_system,
        zoxp_transform_hierarchy,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [none] !transforms3.LocalPosition3D,
        [none] !transforms3.LocalRotation3D,
        [none] !transforms.LocalScale1,
        [none] !transforms3.LocalScale3,
        [none] !transforms.DisableTransform
    );
    add_system_process_counter(world, zox_id(transform3_children_system));
    // TODO: Merge these limits, EulerLimits float4
    // TODO: Put this in a new zoxp_transform_limit phase
    zox_system(
        EulerLimitXSystem,
        zoxp_update,
        [in] EulerLimitX,
        [out] Euler
    );
    zox_system(
        EulerLimitZSystem,
        zoxp_update,
        [in] EulerLimitZ,
        [out] Euler
    );
    // Transform our euler to quaternion
    zox_system(
        EulerOverrideSystem,
        zoxp_update,
        [in] Euler,
        [out] Rotation3D,
        [none] EulerOverride
    );
    // NOTE: No Scales
    zox_system(
        TransformMatrixSystem,
        zoxp_transform_end,
        [in] Position3D,
        [in] Rotation3D,
        [out] transforms.TransformMatrix,
        [none] !transforms.Scale1,
        [none] !transforms3.Scale3,
        [none] !transforms.DisableTransform
    );
    // Scale1
    zox_system(
        TransformMatrixScaleSystem,
        zoxp_transform_end,
        [in] Position3D,
        [in] Rotation3D,
        [in] transforms.Scale1,
        [out] transforms.TransformMatrix,
        [none] !transforms.DisableTransform,
        [none] !transforms3.Scale3,
    );
    zox_system(
        TransformMatrixScale3System,
        zoxp_transform_end,
        [in] Position3D,
        [in] Rotation3D,
        [in] transforms3.Scale3,
        [out] transforms.TransformMatrix,
        [none] !transforms.DisableTransform
    );
}
