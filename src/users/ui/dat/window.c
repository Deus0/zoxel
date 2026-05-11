// all t the spawn data used for spawn
typedef struct {
    entity prefab_header;
    entity prefab_header_text;
    byte2 grid_size;
    byte icon_size;
    byte2 grid_padding;
    byte2 grid_margins;
    entity character;
    entity user_links_id;
} SpawnWindowUsersData;

// for only the button
typedef struct {
    entity prefab;
    FrameTextureData texture;
    int size;
    int index;
    int2 texture_size;
} IconData;

typedef struct {
    LayoutParentData canvas;
    LayoutParentData parent;
    FrameTextureData texture;
    ElementSpawnData element;
    int index;
    int2 texture_size;
} SpawnIcon;
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

typedef struct {
    LayoutParentData canvas;
    LayoutParentData parent;
    ElementSpawnData element;
    SpawnTextData header_zext;
    SpawnHeaderData header;
    SpawnButtonData close_button;
    FrameData frame;
    IconData icon;
    SpawnWindowUsersData window;
} SpawnWindowUsers;
