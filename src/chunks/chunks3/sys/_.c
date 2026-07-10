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
        zoxp_update,
        [in] chunks3.VoxelNodeDirty,
        [out] chunks3.BuildChunkMesh,
        [none] chunks3.Chunk3
    );
    zox_system(
        Chunk3NeighborsMeshTriggerSystem,
        zoxp_update,
        [in] chunks3.VoxelNodeDirty,
        [in] chunks3.ChunkNeighbors,
        [none] chunks3.ChunkTextured
    );
    zox_system(VoxelNodeQueueSystem,
        zoxp_queue_pre_clear,
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
}
