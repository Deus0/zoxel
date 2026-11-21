#include "projection_matrix_system.c"
#include "viewport_resize_system.c"
#include "orthographic.c"
#include "view_projection.c"

void zox_systems_cameras(ecs *world) {
    zox_on_add(
        viewport_resize_event,
        [in] apps.WindowSize,
        [none] apps.App,
        [none] apps.WindowSizeDirty,
    );
    // View Matrixes
    zox_system(
        projection_matrix_system,
        zoxp_update,
        [in] core.PixelSize,
        [in] cameras.FieldOfView,
        [in] cameras.CameraNearDistance,
        [in] cameras.CameraFarDistance,
        [out] cameras.ProjectionMatrix,
        [none] cameras.Perspective,
        [none] !cameras.FieldOfViewAngles
    );
    zox_system(
        projection_angles_system,
        zoxp_update,
        [in] cameras.FieldOfViewAngles,
        [in] cameras.CameraNearDistance,
        [in] cameras.CameraFarDistance,
        [out] cameras.ProjectionMatrix
    );
    zox_system(
        orthographic_matrix_system,
        zoxp_update,
        [in] core.PixelSize,
        [out] cameras.ProjectionMatrix,
        [none] cameras.Orthographic
    );
    // Combine with transform
    zox_system(
        view_projection_matrix_system,
        zoxp_cameras,
        [in] transforms.TransformMatrix,
        [in] cameras.ProjectionMatrix,
        [out] cameras.ViewProjectionMatrix,
        [out] cameras.FrustumCorners,
        [out] transforms3.Position3DBounds,
        [out] cameras.CameraPlanes,
        [none] cameras.Camera,
    );
}
