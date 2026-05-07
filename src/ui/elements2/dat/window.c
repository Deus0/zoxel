// for only the button
typedef struct {
    entity prefab;
    color fill;
    color outline;
} SpawnWindowData;

typedef struct {
    LayoutParentData canvas;
    LayoutParentData parent;
    ElementSpawnData element;
    SpawnButtonData close_button;
    SpawnTextData zext;
    SpawnHeaderData header;
    SpawnWindowData window;
} SpawnWindow;


typedef struct {
    // header
    const char *header_text;
    byte header_font_size;
    byte2 header_padding;
    // list area
    // byte is_scrollbar;
    // todo
    color fill;
    color outline;
    // output
    // Children* children;
} SpawnWindow2;
