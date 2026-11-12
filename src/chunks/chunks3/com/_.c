zox_tag(Chunk3);
zox_tag(NoiseChunk);
zox_tag(ChunkDebugger);
zox_tag(ColorChunk);
zox_tag(LinkChunk);
zox_tag(VoxelNodeDirty);
zox_tag(VoxelNodePostDirty);
zox_tag(ChunkUpdate);
zoxc_int3(ChunkPosition);
zoxc_int3(ChunkSize);
zoxc_byte(ChunkLodDirty);
zox_tag(BlocksSpawned);
// weak: where entities are, not who owns them, so unloading a chunk never deletes what walked into it
//  - npcs are owned by their spawn chunk (ChunkCharacters), the player by its game
zoxc_arrayd_with_remove(ChunkEntities, entity)
zox_tag(ChunkLink);        // links to the chunk an entity is in, which holds it in its ChunkEntities

// Removes an entity from its old chunks ChunkEntities, when moved, unlinked or destroyed
void on_unlink_ChunkLink(ecs* world, entity e, entity chunk) {
    if (!zox_valid(chunk) || !zox_has(chunk, ChunkEntities)) {
        return;
    }
    ChunkEntities* entities = zox_getm(chunk, ChunkEntities);
    remove_from_ChunkEntities(entities, e);
}
#define chunk_neighbors_length 6
zoxc_array(ChunkNeighbors, entity, chunk_neighbors_length);
zoxc_float(RaycastRange);
#include "voxels.c"
#include "sides.c"
#include "queue.c"
#include "damage.c"
// Voxes
zox_tag(Vox);
zox_tag(VoxLods);       // a single max detail vox, that has a child VoxLodMesh per depth
zox_tag(VoxLodMesh);    // a mesh built from its parent VoxLods at its RenderDepth
// zoxc_byte(ChunkLod);    // we using this??!?!
zoxc_hashmap(ChunkLinks, int3)
// operations
zoxc_byte(CloneVox);
zox_tag(CloneVoxLink);
zox_tag(VoxLink);          // links to a child vox that holds voxel data, i.e. a characters vox
zox_tag(VoxOwner);         // has a child vox, linked with VoxLink
zoxc_state(CombineVox);
zoxc_entitiesw(CombineList);
zoxc_arrayd(CombinePositions, byte3);
zoxc_arrayd(CombineDepths, byte);   // depth to sample each CombineList vox at

// ChunkLodDirty
// - chunk_lod_system
// - chunk_mesh_spawn2_system
// - chunk_mesh_transition_system
#define zox_chunk_lod_dirty_start 1
#define zox_chunk_lod_dirty_generate 1
#define zox_chunk_lod_dirty_generating 2
#define zox_chunk_lod_dirty_spawn 3
#define zox_chunk_lod_dirty_toggle 4
#define zox_chunk_lod_dirty_end 0

void zox_components_chunks3(ecs* world) {
    // zoxd_nf_tag
    // Types
    zoxd_tag(Chunk3);
    zoxd_tag(ColorChunk);
    zoxd_tag(LinkChunk);
    zoxd_tag(ChunkDebugger);
    zoxd_tag(NoiseChunk);
    zoxd_tag(Vox);
    zoxd_tag(VoxLods);
    zoxd_tag(VoxLodMesh);
    // Events
    zoxd_tag(VoxelNodeDirty);
    zoxd_tag(VoxelNodePostDirty);
    zoxd_tag(ChunkUpdate);
    // Data
    zoxd_byte(ChunkLodDirty);
    zoxd_tag(BlocksSpawned);
    zoxd_int3(ChunkPosition);
    zoxd_int3(ChunkSize);
    // Links
    zoxd(ChunkNeighbors);
    // Pointers
    zoxd_octree(VoxelNode);
    zoxd(VoxelNodeLock);
    zoxd_octree(SidesOctree);
    zoxd(SidesOctreeLock);
    zoxd_entitiesw(ChunkEntities);
    zoxd_nf_link(ChunkLink);
    zox_on_unlink(on_unlink_ChunkLink, ChunkLink);
    zoxd_queue(VoxelNodeQueue);
    // Voxes
    // zoxd_byte(ChunkLod);
    zoxd_hashmap(ChunkLinks);
    // operations
    zoxd_byte(CloneVox);
    zoxd_state(CombineVox);
    zoxd_nf_link(CloneVoxLink);
    zoxd_nf_link(VoxLink);
    zoxd_tag(VoxOwner);
    zoxd_entitiesw(CombineList);
    zoxd_arrayd(CombinePositions);
    zoxd_arrayd(CombineDepths);
}
