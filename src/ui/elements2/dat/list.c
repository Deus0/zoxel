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
