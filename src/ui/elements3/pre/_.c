#include "layout3.c"
#include "element3.c"
#include "frame.c"
#include "glyph.c"
#include "text3D.c"
#include "label3D.c"
#include "popup3D.c"
#include "elementbar3D_front.c"
#include "elementbar3D.c"
#include "canvas3.c"
#include "canvas3_textured.c"
entity prefab_layout3;
entity prefab_layout3_child;
entity prefab_element3;
entity prefab_frame3;;
// ntity prefab_elementbar3D;
// entity prefab_elementbar3D_front;
entity prefab_text3;
entity prefab_zigel3;
entity prefab_label3D;
entity prefab_popup3D;
entity prefab_canvas3;
// entity prefab_canvas3_textured;

void spawn_prefabs_elements3D(ecs *world) {
    // Layouts
    prefab_layout3 = spawn_prefab_layout3(world);
    prefab_layout3_child = spawn_prefab_layout3_child(world, prefab_layout3);
    // Base
    prefab_canvas3 = spawn_prefab_canvas3(world);   // basic transform
    prefab_element3 = spawn_prefab_element3(world, prefab_layout3_child);
    // Elements
    prefab_frame3 = spawn_prefab_frame3(world, prefab_element3);
    // prefab_elementbar3D_front = spawn_prefab_elementbar3D_front(world, prefab_element3);
    prefab_zigel3 = spawn_prefab_glyph(world, prefab_element3);
    prefab_text3 = spawn_prefab_text(world, prefab_layout3_child, prefab_zigel3);
    // Making these uis now
    prefab_popup3D = spawn_prefab_popup3(world, prefab_element3);
    prefab_label3D = spawn_prefab_label3D(world, prefab_frame3);
    // prefab_elementbar3D = spawn_prefab_elementbar3D(world, prefab_element3);
    // should use Layout3D!
}

/*typedef struct {
    entity prefab;
    entity parent;
    float3 position;
    byte alignment;         // mesh_alignment
    byte2 padding;          // around zigels
    const char* text;
} Text3DData;
typedef struct {
    entity prefab;
    entity parent;
    byte zigel_index; // zox character code
    color fill_color;
    color outline_color;
    byte font_thickness;
    byte font_outline_thickness;
    byte font_outline;
    byte resolution;
    float3 position;
    byte render_disabled; // zox character code
    float scale;
} Zigel3DData;*/