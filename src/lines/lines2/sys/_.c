#include "element.c"
#include "render.c"
#include "position.c"

void define_systems_line2(ecs* world) {
    zox_system(
        Line2DElementSystem,
        zoxp_transforms,
        [in] LinePosition2,
        [out] LineData2D,
        [none] Line2D
    );
    zox_system(
        Line2PositionSystem,
        zoxp_transforms,
        [in] layouts2.LayoutPositionDirty,
        [in] layouts2.CanvasPosition,
        [in] layouts2.LayoutSize
    );
    // zox_render3D_plus_system(
    zox_render3D_system(
        Line2DRenderSystem,
        [in] LineData2D,
        [in] lines.LineThickness,
        [in] colorz.Color,
        [in] layouts2.Layer2D,
        [none] Line2D,
        [none] !ElementLine2D
    );
    zox_render2D_system(
        ElementLine2DRenderSystem,
        [in] LineData2D,
        [in] lines.LineThickness,
        [in] colorz.Color,
        [in] layouts2.Layer2D,
        [none] Line2D,
        [none] ElementLine2D
    );
}
