#include "lines.c"
#include "quads.c"
#include "cubes.c"
#include "render_depths.c"

void define_systems_lines3(ecs* world) {
    zox_render3_system(
        2,
        Line3DRenderSystem,
        [in] lines3.LineData3D,
        [in] lines.LineThickness,
        [in] colorz.Color,
        [none] lines3.Line3D
    );
    zox_render3_system(
        2,
        QuadLineRenderSystem,
        [in] lines.LineThickness,
        [in] colorz.Color,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [in] lines3.QuadLineSize,
        [none] lines3.QuadLines
    );
    // todo: make overlay layer in render stack
    zox_render3_system(
        2,
        CubeLineRenderSystem,
        [in] lines3.DebugCubeLines,
        [in] lines.LineThickness,
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
}
