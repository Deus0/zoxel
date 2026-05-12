#include "euler_override.c"
#include "euler_limit_x.c"
#include "euler_limit_z.c"
#include "transform_matrix.c"
#include "transform_matrix_scale.c"
#include "camera_transform_matrix.c"
#include "shadow.c"
#include "children_new.c"

void zox_define_systems_transforms3(ecs *world) {
    // New hieerarchys using flecs
    // NOTE: Moved before transforms_stage so matricies can update after we have processed position/rotatios
    // NOTE: So normal EcsOnUpdate can set position/rotations without worrying about children
    zox_system(
        PositionRotation3System,
        zox_transforms_stage - 1,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D
    );
    // TODO: Merge these limits, EulerLimits float4
    zox_system(
        EulerLimitXSystem,
        EcsOnUpdate,
        [in] EulerLimitX,
        [out] Euler
    );
    zox_system(
        EulerLimitZSystem,
        EcsOnUpdate,
        [in] EulerLimitZ,
        [out] Euler
    );

    // Transform our euler to quaternion
    zox_system(
        EulerOverrideSystem,
        EcsOnUpdate,
        [in] Euler,
        [out] Rotation3D,
        [none] EulerOverride
    );

    zox_system(
        TransformMatrixSystem,
        zox_transforms_stage,
        [in] Position3D,
        [in] Rotation3D,
        [out] transforms.TransformMatrix,
        [none] !transforms.Scale1D
    );

    zox_system(
        TransformMatrixScaleSystem,
        zox_transforms_stage,
        [in] Position3D,
        [in] Rotation3D,
        [in] transforms.Scale1D,
        [out] transforms.TransformMatrix
    );

    // TODO: Add Lerp Slower Follow
    zox_system(
        ShadowPositionSystem,
        zox_transforms_stage,
        [in] transforms3.ShadowLink,
        [out] transforms3.Position3D
    );
}
