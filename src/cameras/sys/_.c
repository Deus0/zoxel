#include "projection_matrix_system.c"
#include "viewport_resize_system.c"
#include "orthographic.c"
#include "view_projection.c"

void define_systems_cameras(ecs *world) {
    zox_system(
        ViewportResizeSystem,
        zoxp_update,
        [in] apps.WindowSizeDirty,
        [in] apps.WindowSize,
        [none] apps.App
    );
    // View Matrixes
    zox_system(
        ProjectionMatrixSystem,
        zoxp_update,
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
    // Combine with transform
    zox_system(
        ViewProjectionMatrixSystem,
        zoxp_cameras,
        [in] transforms.TransformMatrix,
        [in] ProjectionMatrix,
        [out] ViewProjectionMatrix,
        [out] FrustumCorners,
        [out] transforms3.Position3DBounds,
        [out] CameraPlanes,
        [none] Camera
    );
    /*zox_system(
        CameraFrustumSystem,
        zoxp_cameras,
        [in] ViewProjectionMatrix,
        [out] FrustumCorners,
        [out] transforms3.Position3DBounds,
        [out] CameraPlanes,
        [none] Camera
    );*/
}
