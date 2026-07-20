zox_tag(Chunk3);
zox_tag(NoiseChunk);
zox_tag(ChunkDebugger);
zox_tag(ColorChunk);
zox_tag(ChunkTextured);
zox_tag(LinkChunk);
zoxc_int3(ChunkPosition);
zoxc_int3(ChunkSize);
zoxc_state(ChunkDirty);
zoxc_byte(ChunkLodDirty);
zoxc_byte(BuildChunkMesh);
zoxc_byte(BlocksSpawned);
zoxc_entities(ChunkEntities)
zoxc_link(ChunkLink, entity, ChunkEntities)
#define chunk_neighbors_length 6
zoxc_array(ChunkNeighbors, entity, chunk_neighbors_length);
zoxc_float(RaycastRange);
#include "voxels.c"
#include "sides.c"
#include "queue.c"
#include "damage.c"

#define zox_chunk_lod_dirty_start 1
#define zox_chunk_lod_dirty_octree 1
#define zox_chunk_lod_dirty_spawn 2
#define zox_chunk_lod_dirty_toggle 3
#define zox_chunk_lod_dirty_enable 3
#define zox_chunk_lod_dirty_disable 4
#define zox_chunk_lod_dirty_end 0

void define_components_chunks3(ecs* world) {
    zoxd_tag(Chunk3);
    zoxd_tag(ColorChunk);
    zoxd_tag(ChunkTextured);
    zoxd_tag(LinkChunk);
    zoxd_tag(ChunkDebugger);
    zoxd_tag(NoiseChunk);
    zoxd_node(VoxelNode);
    zoxd_octree(SidesOctree);
    zoxd_byte(ChunkLodDirty);
    zoxd_byte(VoxelNodeDirty);
    zoxd_byte(VoxelNodeEdited); // Marks if Dirty from Users
    zoxd_byte(BlocksSpawned);
    zoxd_byte(BuildChunkMesh);
    zoxd_state(ChunkDirty);
    // zoxd_state(SidesOctreeDirty);
    zoxd_int3(ChunkPosition);
    zoxd_int3(ChunkSize);
    zoxd(ChunkNeighbors);
    zoxd_entities(ChunkEntities);
    zoxd_links(ChunkLink);
    zoxd_queue(VoxelNodeQueue);
}
