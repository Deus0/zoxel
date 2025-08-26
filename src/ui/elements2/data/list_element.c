typedef struct {
    byte type;
    const char* text;
    char* save_path;
    // sliders
    float value;
    float2 value_bounds;
    // todo: consolidate ui events using void*
    ClickEvent on_click;
    SlideEvent on_slide;
} SpawnListElement;