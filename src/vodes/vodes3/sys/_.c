#include "removed.c"
#include "despawn.c"
#include "spawn.c"
#include "lods.c"
#include "block_damage.c"
#include "block_health_overlay.c"

void define_systems_vodes3(ecs* world) {
    // NOTE: Writes to VoxelNode
    zox_system(
        VodesDespawnSystem,
        zoxp_voxels_write,
        [in] chunks3.VoxelNodeDirty,
        [in] rendering.RenderDistanceDirty,
        [in] rendering.RenderDepth,
        [out] chunks3.VoxelNode,
        [out] chunks3.BlocksSpawned
    );
    // NOTE: Writes to VoxelNode
    zox_system(
        VodesRemoveSystem,
        zoxp_voxels_write,
        [in] chunks3.VoxelNodeDirty,
        [in] chunks3.BlocksSpawned,
        [out] chunks3.VoxelNode
    );
    zox_system(
        VodesLodSystem,
        EcsOnUpdate,
        [in] rendering.RenderDistanceDirty,
        [in] rendering.RenderDistance,
        [in] chunks3.VoxelNode,
        [in] chunks3.BlocksSpawned
    );
    zox_system(
        BlockHealthOverlaySystem,
        EcsOnUpdate,
        [in] stats.StatValue,
        [in] stats.StatValueMax,
        [none] vodes3.WorldBlock
    );
    zox_system_1(
        VodesSpawnSystem,
        zoxp_mainthread,
        [in] chunks3.VoxelNodeDirty,
        // [in] rendering.RenderDistanceDirty,
        [in] chunks.NodeDepth,
        [in] rendering.RenderDisabled,
        [in] rendering.RenderDepth,
        [in] rendering.RenderDistance,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
        [out] chunks3.VoxelNode,
        [out] chunks3.BlocksSpawned
    );
    zox_system_1(
        BlockDamageQueueSystem,
        zoxp_queue_add,
        [in] chunks3.ChunkPosition,
        [in] chunks3.VoxelNode,
        [in] chunks.NodeDepth,
        [in] blocks.BlockManagerLink,
        [out] blocks.BlockDamageQueue,
        [none] chunks3.Chunk3
    );
}
