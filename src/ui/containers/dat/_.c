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

int2 calculate_grid_size(byte2 grid_size, byte icon_size, byte2 padding, byte2 margins) {
    return (int2) {
        padding.x * (grid_size.x - 1)
        + icon_size * grid_size.x
        + margins.x * 2,
        padding.y * (grid_size.y - 1)
        + icon_size * grid_size.y
        + margins.y * 2
    };
}

int2 calculate_grid_window_size(SpawnWindowUsersData data, int header_height) {
    return (int2) {
        data.grid_padding.x * (data. grid_size.x - 1)
        + data.icon_size * data. grid_size.x
        + data.grid_margins.x * 2,
        data.grid_padding.y * (data. grid_size.y - 1)
        + data.icon_size * data. grid_size.y
        + data.grid_margins.y * 2
        + header_height
    };
}
