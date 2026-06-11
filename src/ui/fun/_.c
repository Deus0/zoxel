#include "anchor.c"
#include "canvas.c"
#include "element_render.c"
#include "toggle.c"
#include "resize.c"
#include "layers.c"
#include "canvas_fading.c"
#include "set_children.c"
#include "arguments.c"

// move to hlp sub folder
entity get_linked_canvas(ecs* world, entity e) {
    return zox_valid(e) && zox_has(e, CanvasLink) ? zox_gett_value(e, CanvasLink) : 0;
}

void initialize_element(ecs *world, entity e, entity parent, entity canvas, int2 position, int2 size, int2 tsize, float2 anchor, byte layer) {
    if (!zox_valid(e)) {
        zox_loge("Invalid e in [initialize_element]");
        return;
    }
    zox_set_parent(world, e, parent);
    zox_set(e, LayoutPosition, { position });
    zox_set(e, Anchor, { anchor });
    zox_set(e, LayoutSize, { size });
    zox_set(e, Layer2D, { layer });
    if (canvas == parent) {
        zox_set(canvas, WindowToTop, { e });
    }
    zox_set(e, TextureSize, { tsize });
}
