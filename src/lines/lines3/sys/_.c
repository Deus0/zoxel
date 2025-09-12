#include "line3D_render.c"
#include "cube_line_render.c"
#include "render_depths.c"

void define_systems_lines3(ecs* world) {
    zox_render3D_plus_system(
        Line3DRenderSystem,
        [in] LineData3D,
        [in] lines.LineThickness,
        [in] colorz.Color,
        [none] Line3D
    );
    // todo: make overlay layer in render stack
    zox_render3D_plus_system(
        CubeLineRenderSystem,
        [in] DebugCubeLines,
        [in] CubeLinesThickness,
        [in] colorz.Color,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [in] generic.Bounds3D,
        [none] CubeLines
    );
    zox_system(
        RenderDepthColorSystem,
        EcsOnUpdate,
        [in] DebugCubeLines,
        [in] rendering.RenderDepth,
        [in] rendering.RenderDisabled,
        [out] colorz.Color,
        [none] CubeLines
    );
}