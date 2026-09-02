// when spawning Text
typedef struct {
    const char* text;
    byte font_resolution;           // texture resolution
    byte font_size;                 // zigel_size
    byte font_thickness;            // zigel_size
    byte font_outline_thickness;
    byte alignment;                 // mesh_alignment
    byte2 margins;                  // around zigels
    color font_fill_color;
    color font_outline_color;
} SpawnTextData;

typedef struct {
    LayoutParentData canvas;
    LayoutParentData parent;
    ElementSpawnData element;
    SpawnTextData zext;
} SpawnText;
