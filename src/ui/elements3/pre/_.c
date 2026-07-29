/*void set_element_properties(ecs *world, entity e, entity parent, entity canvas, int2 pixel_position, int2 pixel_size, float2 anchor, byte layer, float2 position2, int2 pixel_positionv) {
    zox_set(e, Anchor, { anchor });
    zox_set(e, Layer2D, { layer });
    zox_set(e, LayoutSize, { pixel_size });
    zox_set(e, LayoutPosition, { pixel_position });
    zox_set(e, CanvasLink, { canvas });
    zox_set_parent(world, e, parent);
    if (canvas == parent) {
        zox_set(canvas, WindowToTop, { e });
    }
}*/

#include "layout3.c"
#include "element3.c"
#include "zigel3D.c"
#include "text3D.c"
#include "label3D.c"
#include "popup3D.c"
#include "elementbar3D_front.c"
#include "elementbar3D.c"
#include "canvas3.c"
#include "canvas3_textured.c"
entity prefab_layout3;
entity prefab_element3;
entity prefab_elementbar3D;
entity prefab_elementbar3D_front;
entity prefab_text3D;
entity prefab_zigel3D;
entity prefab_label3D;
entity prefab_popup3D;
entity prefab_canvas3;
// entity prefab_canvas3_textured;

void spawn_prefabs_elements3D(ecs *world) {
    // Base
    prefab_layout3 = spawn_prefab_layout3(world);
    prefab_element3 = spawn_prefab_element3(world, prefab_layout3);
    // Elements
    prefab_elementbar3D_front = spawn_prefab_elementbar3D_front(world, prefab_element3);
    prefab_text3D = spawn_prefab_text3D(world, prefab_layout3);
    prefab_zigel3D = spawn_prefab_zigel3D(world, prefab_element3);
    // Making these uis now
    prefab_popup3D = spawn_prefab_popup3(world, prefab_element3);
    prefab_label3D = spawn_prefab_label3D(world, prefab_element3);
    prefab_elementbar3D = spawn_prefab_elementbar3D(world, prefab_element3);
    // should use Layout3D!
    prefab_canvas3 = spawn_prefab_canvas3(world);
    // prefab_canvas3_textured = spawn_prefab_canvas3_textured(world, prefab_canvas3);
}
