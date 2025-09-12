typedef struct {
    entity prefab;
    byte render_disabled;
    int2 position;
    int2 size;
    // float2 anchor;
    // byte layer;
} Element3DData;

typedef struct {
    float trail_offset;
    entity ui_holder;
    entity prefab;
    byte render_disabled;
    color base_color;
    color outline_color;
} SpawnDataElement3D;

typedef struct {
    float percentage;
    entity ui_holder;
    Element3DData backbar;
    Element3DData frontbar;
    float trail_offset;
} SpawnDataElementbar3D;

