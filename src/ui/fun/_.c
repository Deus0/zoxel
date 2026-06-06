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
    initialize_layout2(world, e, parent, canvas, position, size, anchor, layer);
    zox_set(e, TextureSize, { tsize });
}
