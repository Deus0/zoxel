zox_tag(Chunk3);
zoxc_int3(ChunkPosition);
zoxc_int3(ChunkSize);
#include "voxel_node.c"
zoxc_state(ChunkDirty);
zoxc_state(ChunkMeshDirty);
zoxc_byte(BlocksSpawned);
zoxc_entities(ChunkEntities)
zoxc_link(ChunkLink, entity, ChunkEntities)
#define chunk_neighbors_length 6
zoxc_array(ChunkNeighbors, entity, chunk_neighbors_length);
zox_tag(ChunkDebugger);
zox_tag(ColorChunk);
zox_tag(ChunkTextured);
zox_tag(LinkChunk);
zox_tag(DisableReverseLinkChunk);
zoxc_float(RaycastRange);
#include "sides.c"
zox_tag(NoiseChunk);

void define_components_chunks3(ecs* world) {
    zoxd_tag(Chunk3);
    zoxd_node(VoxelNode);
    zoxd_queue(VoxelNodeQueue);
    zoxd_int3(ChunkPosition);
    zoxd_int3(ChunkSize);
    zoxd_byte(VoxelNodeDirty);
    zoxd_byte(VoxelNodeEdited); // Marks if Dirty from Users
    zoxd_byte(VoxelNodeLoaded);
    zoxd_state(VoxelNodeGenerated);
    zoxd_state(ChunkDirty);
    zoxd_state(ChunkMeshDirty);
    // zoxd_node(SidesOctree);
    zoxd_octree(SidesOctree);
    zoxd_state(SidesOctreeDirty);
    zoxd_byte(BlocksSpawned);
    zoxd(ChunkNeighbors);
    zoxd_entities(ChunkEntities);
    zox_define_links_component(ChunkLink);
    zoxd_tag(ColorChunk);
    zoxd_tag(ChunkTextured);
    zoxd_tag(LinkChunk);
    zoxd_tag(DisableReverseLinkChunk);
    zoxd_tag(ChunkDebugger);
    zoxd_tag(NoiseChunk);
}
