#include "element.c"
#include "render.c"
#include "position.c"
#include "new.c"

void define_systems_line2(ecs* world) {
    zox_render2D_system(
        ElementLine2DRenderSystem,
        [in] transforms2.LinePoints2,
        [in] lines.LineThickness,
        [in] colorz.Color,
        [in] layouts.Layer2D,
        [none] Line2D,
        [none] ElementLine2D
    );
}
