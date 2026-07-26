#include "element.c"
#include "render.c"
#include "position.c"
#include "new.c"

void define_systems_line2(ecs* world) {
    /*zox_system(
        Line2DElementSystem,
        zoxp_transforms,
        [in] LinePosition2,
        [out] LineData2D,
        [none] Line2D
    );*/
    /*zox_system(
        Line2PositionSystem,
        zoxp_transforms,
        [in] lines2.LineLocalPositionDirty,
        [in] lines2.LineAnchor,
        [in] lines2.LineLocalPosition2,
        [out] lines2.LinePosition2
    );*/
    // zox_render3D_plus_system(
    /*zox_render3_system(
        0,
        Line2DRenderSystem,
        [in] LineData2D,
        [in] lines.LineThickness,
        [in] colorz.Color,
        [in] layouts2.Layer2D,
        [none] Line2D,
        [none] !ElementLine2D
    );*/
    zox_render2D_system(
        ElementLine2DRenderSystem,
        [in] LinePoints2,
        [in] lines.LineThickness,
        [in] colorz.Color,
        [in] layouts2.Layer2D,
        [none] Line2D,
        [none] ElementLine2D
    );
}
