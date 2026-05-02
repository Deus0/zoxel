#include "line3D_render.c"
#include "cube_line_render.c"
#include "render_depths.c"
#include "quad_renderer.c"

void define_systems_lines3(ecs* world) {
    zox_render3D_plus_system(
        Line3DRenderSystem,
        [in] lines3.LineData3D,
        [in] lines.LineThickness,
        [in] colorz.Color,
        [none] lines3.Line3D
    );
    // todo: make overlay layer in render stack
    zox_render3D_plus_system(
        CubeLineRenderSystem,
        [in] lines3.DebugCubeLines,
        [in] lines3.CubeLinesThickness,
        [in] colorz.Color,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [in] transforms3.Bounds3D,
        [none] CubeLines
    );
    add_system_process_counter(world, zox_id(CubeLineRenderSystem));

    zox_system(
        RenderDepthColorSystem,
        EcsOnUpdate,
        [in] DebugCubeLines,
        [in] rendering.RenderDepth,
        [in] rendering.RenderDisabled,
        [out] colorz.Color,
        [none] CubeLines
    );
    zox_render3D_plus_system(
        QuadLineRenderSystem,
        [in] lines.LineThickness,
        [in] colorz.Color,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [in] lines3.QuadLineSize,
        [none] lines3.QuadLines
    );
}
