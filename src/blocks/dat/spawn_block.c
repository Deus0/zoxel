typedef struct {
    entity prefab;
    entity prefab_world_block;
    char *name;
    byte index;
    byte model;
    byte textures;
    char *texture_filename;
    byte disable_collision; // enabled by default
    color color;
    entity tag;
    entity texture_tag;
    // generation
    int seed;
    // textures
    entity prefab_texture;
    // vox
    entity vox;
    byte bake_vox;
    byte vox_offset;
} SpawnBlock;