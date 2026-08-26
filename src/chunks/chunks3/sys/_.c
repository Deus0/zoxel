#include "cleanup.c"
#include "mesh_trigger.c"
#include "queue.c"
#include "clear.c"
#include "sides.c"

void define_systems_chunks3(ecs *world) {
    // ColoredChunk, triggers mesh to update
    zox_system(
        ColoredChunkMeshTriggerSystem,
        zoxp_update,
        [none] chunks.Chunk,
        [none] chunks3.ColorChunk,
        [none] chunks3.VoxelNodePostDirty,
    );
    zox_system(
        VoxelUpdateQueueSystem,
        zoxp_queue_pre_clear,
        [in] chunks.NodeDepth,
        [out] chunks3.VoxelNodeQueue,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeEdited,
        [none] chunks3.Chunk3,
    );
    // NOTE: Removes Dirty at end of frame
    zox_system(
        VoxelOctreeOptimizeSystem,
        zoxp_destroy, // zoxp_update,
        [out] chunks3.VoxelNode,
        [none] chunks3.VoxelNodeDirty,
        [none] chunks3.Chunk3
    );
    zox_system(
        VoxelNodePostDirtySystem,
        zoxp_destroy,
        [none] chunks3.VoxelNodePostDirty,
    );
}
