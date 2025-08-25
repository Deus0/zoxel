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