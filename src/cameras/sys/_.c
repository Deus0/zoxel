#include "projection_matrix_system.c"
#include "view_matrix_system.c"
#include "viewport_resize_system.c"
#include "frustum_d3.c"
// #include "frustum_f3.c"
#include "orthographic.c"

void define_systems_cameras(ecs *world) {
    zox_system(
        ViewportResizeSystem,
        zoxp_update,
        [in] apps.WindowSizeDirty,
        [in] apps.WindowSize,
        [none] apps.App
    );
    zox_system(
        ProjectionMatrixSystem,
        zoxp_update, // zoxp_cameras,
        [in] core.PixelSize,
        [in] FieldOfView,
        [in] CameraNearDistance,
        [out] ProjectionMatrix,
        [none] Perspective
    );
    zox_system(
        OrthographicMatrixSystem,
        zoxp_update,
        [in] core.PixelSize,
        //[in] FieldOfView,
        //[in] CameraNearDistance,
        [out] ProjectionMatrix,
        [none] Orthographic
    );
    zox_system(
        ViewProjectionMatrixSystem,
        zoxp_cameras,
        [in] transforms.TransformMatrix,
        [in] ProjectionMatrix,
        [out] ViewProjectionMatrix
    );
    zox_system(
        CameraFrustumSystem,
        zoxp_cameras,
        [in] ViewProjectionMatrix,
        [out] FrustumCorners,
        [out] transforms3.Position3DBounds,
        [out] CameraPlanes,
        [none] Camera
    );
}
