#include "euler_override.c"
#include "parent_position.c"
#include "parent_rotation.c"
#include "euler_limit_x.c"
#include "euler_limit_z.c"
#include "transform_matrix.c"
#include "transform_matrix_scale.c"
#include "camera_transform_matrix.c"
#include "shadow.c"
#include "children.c"

void zox_define_systems_transforms3(ecs *world) {
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
    zox_system(
        EulerOverrideSystem,
        EcsOnUpdate,
        [in] Euler,
        [out] Rotation3D,
        [none] EulerOverride
    );

    /*zox_system(
        ParentRotationSystem,
        zox_transforms_stage,
        [in] hierarchys.ParentLink,
        [in] LocalRotation3D,
        [out] Rotation3D
    );
    zox_system(
        ParentPositionSystem,
        zox_transforms_stage,
        [in] hierarchys.ParentLink,
        [in] LocalPosition3D,
        [out] Position3D
    );*/
    zox_system(
        ChildrenPositionSystem,
        zox_transforms_stage,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [in] hierarchys.Children
    );


    zox_system(
        TransformMatrixSystem,
        zox_transforms_stage,
        [in] Position3D,
        [in] Rotation3D,
        [out] TransformMatrix,
        [none] !transforms.Scale1D
    );
    zox_system(
        TransformMatrixScaleSystem,
        zox_transforms_stage,
        [in] Position3D,
        [in] Rotation3D,
        [in] transforms.Scale1D,
        [out] TransformMatrix
    );
    // TODO: Add Lerp Slower Follow
    zox_system(
        ShadowPositionSystem,
        zox_transforms_stage,
        [in] transforms3.ShadowLink,
        [out] transforms3.Position3D
    );
}
