#include "euler_override.c"
#include "euler_limit_x.c"
#include "euler_limit_z.c"
#include "transform_matrix.c"
#include "transform_matrix_scale.c"
#include "camera_transform_matrix.c"
#include "shadow.c"
#include "children.c"

void zox_define_systems_transforms3(ecs *world) {
    zox_system(
        TransformChildrenSystem,
        zoxp_transforms,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [none] !transforms3.LocalPosition3D,
        [none] !transforms3.LocalRotation3D,
        [none] !transforms.DisableTransform
    );
    add_system_process_counter(world, zox_id(TransformChildrenSystem));
    // TODO: Merge these limits, EulerLimits float4
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
        zoxp_transforms,
        [in] Position3D,
        [in] Rotation3D,
        [out] transforms.TransformMatrix,
        [none] !transforms.Scale1D,
        [none] !transforms.DisableTransform
    );
    // Scale1D
    zox_system(
        TransformMatrixScaleSystem,
        zoxp_transforms,
        [in] Position3D,
        [in] Rotation3D,
        [in] transforms.Scale1D,
        [out] transforms.TransformMatrix,
        [none] !transforms.DisableTransform
    );
    // TODO: Add Lerp Slower Follow
    /*zox_system(
        ShadowPositionSystem,
        zoxp_transforms,
        [in] transforms3.ShadowLink,
        [out] transforms3.Position3D
    );*/
}
