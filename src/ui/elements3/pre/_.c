void set_element_properties(ecs *world, entity e, entity parent, entity canvas, int2 pixel_position, int2 pixel_size, float2 anchor, byte layer, float2 position2, int2 pixel_positionv) {
    zox_set(e, Anchor, { anchor });
    zox_set(e, Layer2D, { layer });
    zox_set(e, LayoutSize, { pixel_size });
    zox_set(e, LayoutPosition, { pixel_position });
    zox_set(e, CanvasLink, { canvas });
    zox_set(e, ParentLink, { parent });

    if (canvas == parent) {
        on_child_added(world, canvas, e);
        zox_set(canvas, WindowToTop, { e });
    }
}

#include "canvas3.c"
#include "canvas3_textured.c"
#include "element_world_child.c"
#include "element3D_invisible.c"
#include "zigel3D.c"
#include "text3D.c"
#include "label3D.c"
#include "popup3D.c"
#include "elementbar3D_front.c"
#include "elementbar3D.c"

entity prefab_canvas3;
entity prefab_canvas3_textured;
entity prefab_element3D_child;
entity prefab_element3D_invisible;
entity prefab_elementbar3D;
entity prefab_elementbar3D_front;
entity prefab_text3D;
entity prefab_zigel3D;
entity prefab_label3D;
entity prefab_popup3D;

void spawn_prefabs_elements3D(ecs *world) {

    prefab_canvas3 = spawn_prefab_canvas3(world);
    prefab_canvas3_textured = spawn_prefab_canvas3_textured(world, prefab_canvas3);

    prefab_element3D_child = spawn_prefab_element3D_child(world);
    prefab_element3D_invisible = spawn_prefab_element3D_invisible(world);
    prefab_elementbar3D = spawn_prefab_elementbar3D(world, prefab_canvas3_textured);
    prefab_elementbar3D_front = spawn_prefab_elementbar3D_front(world, prefab_element3D_child);
    prefab_text3D = spawn_prefab_text3D(world, prefab_element3D_invisible);
    prefab_zigel3D = spawn_prefab_zigel3D(world, prefab_element3D_child);
    prefab_label3D = spawn_prefab_label3D(world, prefab_canvas3_textured);
    // should use Layout3D!
    prefab_popup3D = spawn_prefab_popup3(world, prefab_canvas3);
}
