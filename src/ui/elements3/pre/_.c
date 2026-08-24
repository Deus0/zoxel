#include "element3.c"
#include "frame.c"
#include "glyph.c"
#include "label3D.c"
#include "popup3D.c"
#include "elementbar3D_front.c"
#include "elementbar3D.c"
#include "canvas3.c"
#include "canvas3_textured.c"
entity prefab_element3;
entity prefab_frame3;;
entity prefab_text3;
entity prefab_zigel3;
entity prefab_label3D;
entity prefab_popup3D;
entity prefab_canvas3;

void spawn_prefabs_elements3D(ecs *world) {
    // Base
    prefab_canvas3 = spawn_prefab_canvas3(world);   // basic transform
    prefab_element3 = spawn_prefab_element3(world, prefab_layout3_child);
    // Elements
    prefab_frame3 = spawn_prefab_frame3(world, prefab_element3);
    prefab_zigel3 = spawn_prefab_glyph(world, prefab_element3);
    prefab_text3 = spawn_prefab_text(
        world,
        prefab_layout3_child,
        prefab_zigel3);
    // Making these uis now
    prefab_popup3D = spawn_prefab_popup3(world, prefab_element3);
    prefab_label3D = spawn_prefab_label3D(world, prefab_frame3);
}