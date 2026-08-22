// #include "projection.c"
// #include "frame.c"
#include "render.c"

void zox_systems_xr(ecs* world) {
    /*zox_system(
        xr_projection_system,
        zoxp_update,
        [in] core.Initialize,
        [in] xr.XrFov,
        [out] cameras.ProjectionMatrix,
    );*/
    /*zox_system_1(
        xr_frame_system,
        zoxp_rendering,
        0
    );*/
    zox_system_1(
        xr_camera_render_system,
        zoxp_rendering,
        0
        /*[in] cameras.ViewMatrix,
        [in] transforms3.Position3D,
        [in] cameras.FieldOfView,
        [in] cameras.ScreenPosition,
        [in] screens.ScreenDimensions,
        [in] colorz.FogColor,
        [in] rendering.FrameBufferLink,
        [none] xr.XrCamera*/
    );
}
