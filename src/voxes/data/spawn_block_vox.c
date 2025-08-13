typedef struct {
    entity prefab;
    entity vox;
    byte block_index;
    byte render_depth;
    byte render_disabled;
    byte3 positionl;
    int3 positionv;
    float3 positionf;
    float scale;
} SpawnBlockVox;
