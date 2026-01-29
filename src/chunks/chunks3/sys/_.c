zox_increment_system_with_reset(VoxelNodeDirty, zox_dirty_end + 1);
#include "cleanup.c"
#include "mesh_trigger.c"
#include "mesh_trigger_neighbor.c"
#include "voxel_node_queue.c"
#include "voxel_node_queue_clear.c"
#include "sides.c"

void define_systems_chunks3(ecs *world) {
    zoxd_system_increment(VoxelNodeDirty);

    zox_system(
        Chunk3MeshTriggerSystem,
        EcsOnLoad,
        [in] chunks3.VoxelNodeDirty,
        [out] chunks3.ChunkMeshDirty
    );
    zox_system(
        Chunk3NeighborsMeshTriggerSystem,
        EcsOnLoad,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNodeDirty,
        [out] chunks3.ChunkMeshDirty
    );

    zox_system(
        VoxelNodeCleanupSystem,
        zoxp_voxels_write,
        [in] chunks3.VoxelNodeDirty,
        [in] chunks.NodeDepth,
        [out] chunks3.VoxelNode
    );

    zox_system(VoxelNodeQueueSystem,
        zoxp_queue_process1,
        [in] chunks.NodeDepth,
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

    // Reduce our air
    zox_system(
        SidesOctreeDirtySystem,
        EcsOnStore,
        [in] chunks3.SidesOctreeDirty,
        [out] chunks3.SidesOctree,
        [none] chunks.Chunk
    );

}
