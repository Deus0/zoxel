#include "cleanup.c"
#include "mesh_trigger.c"
#include "neighbor_trigger.c"
#include "queue.c"
#include "clear.c"
#include "sides.c"
zox_increment_system_with_reset(VoxelNodeDirty, zox_dirty_end + 1);

void define_systems_chunks3(ecs *world) {
    zoxd_system_increment(VoxelNodeDirty);
    zox_system(
        Chunk3MeshTriggerSystem,
        EcsOnUpdate,
        [in] chunks3.VoxelNodeDirty,
        [out] chunks3.ChunkMeshDirty,
        [none] chunks3.Chunk3
    );
    zox_system(
        Chunk3NeighborsMeshTriggerSystem,
        EcsOnUpdate,
        [in] chunks3.ChunkNeighbors,
        [out] chunks3.ChunkMeshDirty,
        [none] chunks3.Chunk3,
        [none] chunks3.ChunkTextured    // only terrain for now
    );
    zox_system(VoxelNodeQueueSystem,
        zoxp_queue_process1,
        [in] chunks.NodeDepth,
        [in] chunks3.VoxelNodeQueue,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeDirty,
        [out] chunks3.VoxelNodeEdited,
        [none] chunks3.Chunk3
    );
    zox_system(
        VoxelOctreeCleanupSystem,
        zoxp_voxels_write,
        [in] chunks3.VoxelNodeDirty,
        [out] chunks3.VoxelNode,
        [none] chunks3.Chunk3
    );
    zox_system(
        VoxelNodeQueueClearSystem,
        zoxp_queue_clear,
        [out] VoxelNodeQueue,
        [none] chunks3.Chunk3
    );
    // Reduce our air
    /*zox_system(
        SidesOctreeDirtySystem,
        EcsOnStore,
        [in] chunks3.SidesOctreeDirty,
        [out] chunks3.SidesOctree,
        [none] chunks.Chunk
    );*/
}
