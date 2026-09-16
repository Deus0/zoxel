#include "scale1.c"
#include "scale2.c"
#include "children.c"

// TODO: Add ANgle
void zox_define_systems_transforms2(ecs* world) {
    zox_system(
        Transform2ChildrenSystem,
        zoxp_transform_hierarchy,
        [in] transforms2.Position2,
        // [in] transforms2.Rotation2,
        [none] !transforms2.LocalPosition2,
        // [none] !transforms2.LocalRotation2,
        [none] !transforms.DisableTransform
    );
    // TODO: Add Rottions to these
    zox_system(
        Transform2Scale1System,
        zoxp_transform_end,
        [in] transforms2.Position2,
        [in] transforms.Scale1,
        [out] transforms.TransformMatrix,
        [none] !transforms2.Rotation2,
        [none] !transforms2.Scale2,
        [none] !transforms.DisableTransform
    );
    zox_system(
        Transform2Rotation2Scale1System,
        zoxp_transform_end,
        [in] transforms2.Position2,
        [in] transforms2.Rotation2,
        [in] transforms.Scale1,
        [out] transforms.TransformMatrix,
        [none] !transforms2.Scale2,
        [none] !transforms.DisableTransform
    );
    zox_system(
        Transform2Scale2System,
        zoxp_transform_end,
        [in] transforms2.Position2,
        [in] transforms2.Scale2,
        [out] transforms.TransformMatrix,
        [none] !transforms2.Rotation2,
        [none] !transforms.DisableTransform
    );
}
