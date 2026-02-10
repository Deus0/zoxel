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

#define list_element_type_button 0
#define list_element_type_slider 1
#define list_element_type_toggle 2
#define list_element_type_label 0
