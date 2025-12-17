// data used when spawning any element

typedef struct {
    color fill_color;
    color outline_color;
} FrameTextureData;

typedef struct {
    LayoutParentData canvas;
    LayoutParentData parent;
    FrameTextureData texture;
    ElementSpawnData element;
} ElementSpawn;
