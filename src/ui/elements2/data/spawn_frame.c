// for only the button
typedef struct {
    entity prefab;
    FrameTextureData texture;
} FrameData;

typedef struct {
    LayoutParentData canvas;
    LayoutParentData parent;
    FrameTextureData texture;
    ElementSpawnData element;
    IconData icon;
} SpawnFrame;