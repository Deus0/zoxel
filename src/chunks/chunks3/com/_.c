// Main Chunk Data
zox_tag(Chunk3);
zoxc_int3(ChunkPosition);
zoxc_int3(ChunkSize);
#include "voxel_node.c"

// States
zoxc_state(ChunkDirty);
zoxc_state(ChunkMeshDirty);
zoxc_state(GenerateChunk);
zoxc_byte(BlocksSpawned);

// Entites inside a Chunk
zoxc_entities(ChunkEntities)
zoxc_link(ChunkLink, entity, ChunkEntities)
// #include "chunk_links.c"

// A Chunks Neighbor Chunks
#define chunk_neighbors_length 6
zoxc_array(ChunkNeighbors, entity, chunk_neighbors_length);

// Properties
zox_tag(NoiseChunk);
zox_tag(ChunkDebugger);
zox_tag(ColorChunk);
zox_tag(ChunkTextured);
zox_tag(LinkChunk);
zox_tag(DisableReverseLinkChunk);

// Misc
zoxc_float(RaycastRange);

void define_components_chunks3(ecs* world) {
    // Main Chunk Voxel Data
    zoxd_tag(Chunk3);
    zoxd_node(VoxelNode);
    zoxd_queue(VoxelNodeQueue);
    zoxd_int3(ChunkPosition);
    zoxd_int3(ChunkSize);

    // States
    zoxd_byte(VoxelNodeDirty);
    zoxd_byte(VoxelNodeEdited);
    zoxd_byte(VoxelNodeLoaded);
    zoxd_state(VoxelNodeGenerated);
    zoxd_state(ChunkDirty);
    zoxd_state(ChunkMeshDirty);
    zoxd_state(GenerateChunk);
    zoxd_byte(BlocksSpawned);

    // A Chunks Neighbor Chunks
    zoxd(ChunkNeighbors);

    // Entities inside a chunk
    zoxd_entities(ChunkEntities);
    zox_define_links_component(ChunkLink);

    // Properties
    zoxd_tag(NoiseChunk);
    zoxd_tag(ColorChunk);
    zoxd_tag(ChunkTextured);
    zoxd_tag(LinkChunk);
    zoxd_tag(DisableReverseLinkChunk);
    zoxd_tag(ChunkDebugger);
}
