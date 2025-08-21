// data used when spawning any element

typedef struct {
    color fill_color;
    color outline_color;
} FrameTextureData;

typedef struct {
    CanvasSpawnData canvas;
    ParentSpawnData parent;
    FrameTextureData texture;
    ElementSpawnData element;
} ElementSpawn;
