zox_increment_system_with_reset(ChunkDirty, chunk_dirty_state_end);
zox_increment_system_with_reset(ChunkMeshDirty, chunk_dirty_state_end);
zox_increment_system_with_reset(GenerateChunk, chunk_generate_state_end);
zox_increment_system_with_reset(VoxelNodeGenerated, chunk_generate_state_end);
// zox_increment_system_with_reset(ChunkLodDirty, chunk_lod_state_end);
zox_increment_system_with_reset(VoxelNodeDirty, zox_dirty_end + 1);
#include "entities_lod.c"
#include "find_neighbor.c"
#include "cleanup.c"
#include "mesh_trigger.c"
#include "mesh_trigger_neighbor.c"
#include "debug.c"
#include "block_scale.c"

void get_chunk_filename(char* out, const int3 position) {
        sprintf(out, "chunk_%i_%i_%i.dat", position.x, position.y, position.z);
}

#include "chunk3_save.c"
#include "chunk3_load.c"
#include "voxel_node_queue.c"

void define_systems_chunks(ecs *world) {
    zoxd_system_increment(ChunkDirty);
    zoxd_system_increment(ChunkMeshDirty);
    zoxd_system_increment(GenerateChunk);
    zoxd_system_increment(VoxelNodeGenerated);

    // zoxd_system_increment(ChunkLodDirty);
    zoxd_system_increment(VoxelNodeDirty);
    zox_system(ChunkEntitiesLodSystem, EcsOnUpdate,
            [in] rendering.RenderDistanceDirty,
            [in] rendering.RenderDistance,
            [in] chunks3.ChunkEntities);
    zox_system(ChunkFindNeighborSystem, EcsOnLoad,
            [in] chunks3.ChunkPosition,
            [in] chunks3.VoxLink,
            [in] rendering.RenderDepth,
            [out] chunks3.ChunkNeighbors,
            [none] ChunkTextured);
    zox_system(Chunk3MeshTriggerSystem, EcsOnUpdate,
            [in] chunks3.VoxelNodeDirty,
            [out] chunks3.ChunkMeshDirty);
    zox_system(Chunk3NeighborsMeshTriggerSystem, EcsOnUpdate,
            [in] chunks3.ChunkNeighbors,
            [in] chunks3.VoxelNodeDirty,
            [out] chunks3.ChunkMeshDirty);
    zox_system(VoxelNodeCleanupSystem, zoxp_voxels_write,
            [in] chunks3.VoxelNodeDirty,
            [in] chunks3.NodeDepth,
            [out] chunks3.VoxelNode);
    // main thread
    zox_system_1(ChunkDebugSystem, zoxp_voxels_read,
            [in] blocks.BlockScale,
            [in] transforms3.Position3D,
            [in] chunks3.VoxelNode,
            [in] chunks3.NodeDepth,
            [in] rendering.RenderDistance,
            [in] chunks3.ChunkNeighbors,
            [none] ChunkDebugger);
    zox_system(Chunk3SaveSystem, EcsOnUpdate,
            [in] chunks3.VoxelNodeEdited,
            [in] chunks3.VoxelNodeDirty,
            [in] chunks3.VoxelNode,
            [in] chunks3.ChunkPosition);
    zox_system(Chunk3LoadSystem, EcsOnUpdate,
            [in] rendering.RenderDistanceDirty,
            [in] chunks3.ChunkPosition,
            [out] chunks3.VoxelNodeDirty,
            [out] chunks3.VoxelNodeEdited,
            [out] chunks3.VoxelNodeLoaded,
            [out] chunks3.VoxelNode,
            [out] chunks3.NodeDepth);
    zox_system(BlockScaleSystem, EcsOnUpdate,
            [in] rendering.RenderDepthDirty,
            [in] rendering.RenderDepth,
            [out] chunks3.VoxLink,
            [out] blocks.BlockScale);
    zox_system(VoxelNodeQueueSystem, zoxp_voxels_write,
            [in] chunks3.NodeDepth,
            [out] chunks3.VoxelNodeQueue,
            [out] chunks3.VoxelNode,
            [out] chunks3.VoxelNodeDirty,
            [out] chunks3.VoxelNodeEdited);
}
