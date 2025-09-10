zox_increment_system_with_reset(VoxelNodeDirty, zox_dirty_end + 1);
#include "entities_lod.c"
#include "find_neighbor.c"
#include "cleanup.c"
#include "mesh_trigger.c"
#include "mesh_trigger_neighbor.c"
#include "debug.c"
#include "block_scale.c"
#include "voxel_node_queue.c"
#include "voxel_node_queue_clear.c"

void define_systems_chunks(ecs *world) {
    zoxd_system_increment(VoxelNodeDirty);

    zox_system(
        ChunkEntitiesLodSystem,
        EcsOnUpdate,
        [in] rendering.RenderDistanceDirty,
        [in] rendering.RenderDistance,
        [in] chunks3.ChunkEntities
    );
    zox_system(
        ChunkFindNeighborSystem,
        EcsOnLoad,
        [in] chunks3.ChunkPosition,
        [in] chunks3.VoxLink,
        [in] rendering.RenderDepth,
        [out] chunks3.ChunkNeighbors,
        [none] ChunkTextured
    );
    zox_system(
        Chunk3MeshTriggerSystem,
        EcsOnUpdate,
        [in] chunks3.VoxelNodeDirty,
        [out] chunks3.ChunkMeshDirty
    );
    zox_system(
        Chunk3NeighborsMeshTriggerSystem,
        EcsOnUpdate,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNodeDirty,
        [out] chunks3.ChunkMeshDirty
    );
    zox_system(
        VoxelNodeCleanupSystem,
        zoxp_voxels_write,
        [in] chunks3.VoxelNodeDirty,
        [in] chunks3.NodeDepth,
        [out] chunks3.VoxelNode
    );

    zox_system(BlockScaleSystem,
        EcsPostLoad,
        [in] rendering.RenderDepthDirty,
        [in] rendering.RenderDepth,
        [out] chunks3.VoxLink,
        [out] blocks.BlockScale
    );
    zox_system(VoxelNodeQueueSystem,
        zoxp_queue_process1,
        [in] chunks3.NodeDepth,
        [in] chunks3.VoxelNodeQueue,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeDirty,
        [out] chunks3.VoxelNodeEdited
    );
    zox_system(
        VoxelNodeQueueClearSystem,
        zoxp_queue_clear,
        [out] VoxelNodeQueue
    );
    // main thread
    zox_system_1(
        ChunkDebugSystem,
        zoxp_voxels_read,
        [in] blocks.BlockScale,
        [in] transforms3.Position3D,
        [in] chunks3.VoxelNode,
        [in] chunks3.NodeDepth,
        [in] rendering.RenderDistance,
        [in] chunks3.ChunkNeighbors,
        [none] ChunkDebugger
    );
}
