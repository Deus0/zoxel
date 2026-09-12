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
        zoxp_update,
        [in] rendering.RenderDistanceDirty,
        [in] rendering.RenderDepth,
        [out] chunks3.VoxelNode,
        [none] chunks3.BlocksSpawned,
    );
    // NOTE: Writes to VoxelNode
    zox_system(
        VodesRemoveSystem,
        zoxp_update,
        [out] chunks3.VoxelNode,
        [none] chunks3.BlocksSpawned,
        [none] chunks3.VoxelNodeDirty,
    );
    zox_system(
        VodesLodSystem,
        zoxp_update,
        [in] rendering.RenderDistanceDirty,
        [in] rendering.RenderDistance,
        [in] chunks3.VoxelNode,
        [none] chunks3.BlocksSpawned,
    );
    zox_system(
        BlockHealthOverlaySystem,
        zoxp_update,
        [in] stats.StatValue,
        [in] stats.StatValueMax,
        [none] vodes3.WorldBlock
    );
    zox_system_1(
        VodesSpawnSystem,
        zoxp_spawn,
        [in] chunks.NodeDepth,
        [in] rendering.RenderDisabled,
        [in] rendering.RenderDepth,
        [in] rendering.RenderDistance,
        [in] transforms3.Position3D,
        [out] chunks3.VoxelNode,
        [none] terrains.TerrainChunk,
    );
    zox_system_1(
        BlockDamageQueueSystem,
        zoxp_spawn,
        [in] chunks3.ChunkPosition,
        [in] chunks3.VoxelNode,
        [in] chunks.NodeDepth,
        [in] blocks.BlockManagerLink,
        [out] blocks.BlockDamageQueue,
        [none] chunks3.Chunk3
    );
}
