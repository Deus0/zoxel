#include "scale1.c"
#include "scale2.c"

// TODO: Add ANgle
void zox_define_systems_transforms2(ecs* world) {
    zox_system(
        Transform2Scale1System,
        zoxp_update,
        [in] transforms2.Position2,
        [in] transforms.Scale1D,
        [out] transforms.TransformMatrix,
        [none] !transforms2.Scale2D,
        [none] !transforms.DisableTransform
    );
    zox_system(
        Transform2Scale2System,
        zoxp_update,
        [in] transforms2.Position2,
        [in] transforms2.Scale2D,
        [out] transforms.TransformMatrix,
        [none] !transforms.DisableTransform
    );
}
