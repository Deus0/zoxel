typedef struct {
    entity prefab;
    const entity player;
    const float3 position;
    const float4 rotation;
    const float3 euler;
    float scale;
    const entity model;
    const byte render_depth;
    const byte render_disabled;
    const entity realm;
    const entity terrain;
    const entity terrain_chunk;
    const int3 chunk_position;
} spawn_character3D_data;