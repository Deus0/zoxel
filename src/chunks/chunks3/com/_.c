zox_tag(Chunk3);
zox_tag(NoiseChunk);
zox_tag(ChunkDebugger);
zox_tag(ColorChunk);
zox_tag(LinkChunk);
zox_tag(VoxelNodeDirty);
zox_tag(VoxelNodePostDirty);
zoxc_int3(ChunkPosition);
zoxc_int3(ChunkSize);
zoxc_byte(ChunkLodDirty);
// (BuildChunkMesh);
zoxc_byte(BlocksSpawned);
// zoxc_byte(VoxelNodeEdited);
zoxc_state(ChunkDirty);
zoxc_entities(ChunkEntities);
zoxc_link(ChunkLink, entity, ChunkEntities)
#define chunk_neighbors_length 6
zoxc_array(ChunkNeighbors, entity, chunk_neighbors_length);
zoxc_float(RaycastRange);
#include "voxels.c"
#include "sides.c"
#include "queue.c"
#include "damage.c"

// ChunkLodDirty
#define zox_chunk_lod_dirty_start 1
#define zox_chunk_lod_dirty_generate 1
#define zox_chunk_lod_dirty_generating 2
#define zox_chunk_lod_dirty_spawn 3
#define zox_chunk_lod_dirty_toggle 4
#define zox_chunk_lod_dirty_end 0

// BuildChunkMesh
//#define zox_build_chunk_mesh_run 1
//#define zox_build_chunk_mesh_lights 2
//#define zox_build_chunk_mesh_end 0

void define_components_chunks3(ecs* world) {
    zoxd_nf_tag(Chunk3);
    zoxd_nf_tag(ColorChunk);
    zoxd_nf_tag(LinkChunk);
    zoxd_nf_tag(ChunkDebugger);
    zoxd_nf_tag(NoiseChunk);
    zoxd_nf_tag(VoxelNodeDirty);
    zoxd_nf_tag(VoxelNodePostDirty);
    // Data
    zoxd_byte(ChunkLodDirty);
    // zoxd_byte(VoxelNodeEdited); // Marks if Dirty from Users
    zoxd_byte(BlocksSpawned);
    zoxd_state(ChunkDirty);
    zoxd_int3(ChunkPosition);
    zoxd_int3(ChunkSize);
    // Links
    zoxd(ChunkNeighbors);
    zoxd_links(ChunkLink);
    // Pointers
    zoxd_octree(VoxelNode);
    zoxd_octree(SidesOctree);
    zoxd_entities(ChunkEntities);
    zoxd_queue(VoxelNodeQueue);
}
