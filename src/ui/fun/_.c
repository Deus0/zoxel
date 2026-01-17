#include "ui_prefab.c"
#include "ui_transform.c"
#include "anchor.c"
#include "canvas.c"
#include "drag.c"
#include "element_render.c"
#include "click.c"
#include "toggle.c"
#include "resize.c"
#include "layers.c"
#include "canvas_fading.c"
#include "set_children.c"
#include "arguments.c"
#include "active.c"

// move to hlp sub folder
entity get_linked_canvas(ecs* world, entity e) {
    return zox_valid(e) && zox_has(e, CanvasLink) ? zox_gett_value(e, CanvasLink) : 0;
}