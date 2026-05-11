#define list_element_type_button 0
#define list_element_type_slider 1
#define list_element_type_toggle 2
#define list_element_type_label 0
#define zox_slider_type_float 0
#define zox_slider_type_int 1

// for only the button
typedef struct {
    entity prefab_zext;
    color fill;
    color outline;
} SpawnButtonData;

// just use for holding atm
typedef struct {
    LayoutParentData canvas;
    LayoutParentData parent;
    ElementSpawnData element;
    SpawnTextData zext;
    SpawnButtonData button;
} SpawnButton;

typedef struct {
    byte type;
    const char* text;
    char* save_path;
    // sliders
    float value;
    float2 value_bounds;
    // todo: consolidate ui events using void*
    ClickEvent on_click;
    ToggleEvent on_toggle;
    SlideEvent on_slide;
} SpawnListElement;

// for only the button
typedef struct {
    SpawnListElement* elements;
    int count;
    int visible_count;
    byte font_size;
    color fill;
    color outline;
    // list spacing
    byte2 button_padding;
    byte2 padding;
    byte2 margins;
    // slider
    int slider_height; // = 64; // now we doing labels
    int slider_padding; // = 64;
} SpawnList;

typedef struct {
    const char* name;
    byte type;
    entity prefab_handle;
    float value;
    float2 bounds;
    byte handle_width;
} SpawnSliderData;

// for only the button
typedef struct {
    entity prefab_zext;
    byte is_close_button;
    // byte margins;
    color color;
} SpawnHeaderData;

typedef struct {
    LayoutParentData canvas;
    LayoutParentData parent;
    ElementSpawnData element;
    SpawnButtonData close_button;
    SpawnTextData zext;
    SpawnHeaderData header;
} SpawnHeader;

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
    const char *header_text;
    byte header_font_size;
    byte2 header_padding;
    color fill;
    color outline;
} SpawnWindow2;