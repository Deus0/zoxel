typedef struct {
    entity prefab;
    // layout2
    int2 position;
    int2 size;
    float2 anchor;
    // rendering
    byte render_disabled;
    byte layer;
    int2 texture_size;
    // outputs
    int2 position_in_canvas;
} ElementSpawnData;