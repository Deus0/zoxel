zox_tag(Chunk);
zox_tag(NoiseChunk);
zox_tag(ChunkDebugger);
zox_tag(ColorChunk);
zox_tag(ChunkTextured);
zox_tag(LinkChunk);
zox_tag(DisableReverseLinkChunk);
zoxc_state(ChunkDirty);
zoxc_state(ChunkMeshDirty);
zoxc_state(GenerateChunk);
zoxc_int3(ChunkPosition);
zoxc_int3(ChunkSize);
zoxc_byte(BlocksSpawned);
zoxc_byte(NodeDepth);
zoxc_float(RaycastRange);

zoxc_entity(VoxLink);
zoxc_entities(ChunkEntities)

zoxc_link(ChunkLink, entity, ChunkEntities)
#define chunk_neighbors_length 6
zoxc_array(ChunkNeighbors, entity, chunk_neighbors_length);
#include "chunk_links.c"
#include "voxel_node.c"

void define_components_chunks(ecs* world) {
    zoxd_entity(VoxLink);
    zoxd_entities(ChunkEntities);
    zoxd_tag(Chunk);
    zoxd_tag(NoiseChunk);
    zoxd_tag(ColorChunk);
    zoxd_tag(ChunkTextured);
    zoxd_tag(LinkChunk);
    zoxd_tag(DisableReverseLinkChunk);
    zoxd_tag(ChunkDebugger);
    zoxd_int3(ChunkPosition);
    zoxd_state(ChunkDirty);
    zoxd_state(ChunkMeshDirty);
    zoxd_int3(ChunkSize);
    zoxd_state(GenerateChunk);
    zoxd_byte(BlocksSpawned);
    zoxd(ChunkNeighbors);
    zox_define_links_component(ChunkLink);
    zoxd_byte(NodeDepth);
    // Voxels
    zoxd_node(VoxelNode);
    zoxd_byte(VoxelNodeDirty);
    zoxd_byte(VoxelNodeEdited);
    zoxd_byte(VoxelNodeLoaded);
    zoxd_state(VoxelNodeGenerated);
    zox_define_hashmap_component(ChunkLinks);
    zoxd_queue(VoxelNodeQueue);
}
