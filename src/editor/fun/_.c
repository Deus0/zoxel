const int inspector_component_size_buffer = 128;
#include "tooltip.c"
#include "shared.c"
#include "inspector.c"
#include "hierarchy.c"
#include "component.c"

void set_editor_window(ecs* world, entity e) {
    byte layer = get_game_overlay_layer() + 2;
    zox_add(e, EditorElement);
    zox_add(e, IgnoreCanvasStack);
    zox_add(e, WindowLayerDirty);
    zox_setv(e, BonusLayer, layer);
}