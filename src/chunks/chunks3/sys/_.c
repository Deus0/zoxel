#include "cleanup.c"
#include "mesh_trigger.c"
#include "queue.c"
#include "clear.c"
#include "sides.c"
// zox_increment_system_with_reset(VoxelNodeDirty, zox_dirty_end + 1);

void define_systems_chunks3(ecs *world) {
    // zoxd_system_increment(VoxelNodeDirty);
    // ColoredChunk
    zox_system(
        ColoredChunkMeshTriggerSystem,
        zoxp_update,
        [in] chunks3.VoxelNodeDirty,
        [out] rendering.BuildMesh,
        [none] chunks3.ColorChunk
    );
    zox_system(
        VoxelUpdateQueueSystem,
        zoxp_queue_pre_clear,
        [in] chunks.NodeDepth,
        [out] chunks3.VoxelNodeQueue,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeDirty,
        [out] chunks3.VoxelNodeEdited,
        [none] chunks3.Chunk3
    );
    zox_system(
        VoxelOctreeOptimizeSystem,
        zoxp_update,
        [in] chunks3.VoxelNodeDirty,
        [out] chunks3.VoxelNode,
        [none] chunks3.Chunk3
    );
}
