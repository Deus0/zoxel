#include "removed.c"
#include "despawn.c"
#include "spawn.c"
#include "lods.c"
#include "block_damage.c"
#include "block_health_overlay.c"
#include "can.c"
#include "settings.c"

void zox_systems_vodes3(ecs* world) {
    // NOTE: Writes to VoxelNode
    zox_system(
        VodesDespawnSystem,
        zoxp_octree_write,
        [in] rendering.RenderDistanceDirty,
        [in] rendering.RenderDepth,
        [out] chunks3.VoxelNode,
        [none] chunks3.BlocksSpawned,
        [none] !vodes.CanVodes,
    );
    // NOTE: Writes to VoxelNode
    zox_system(
        VodesRemoveSystem,
        zoxp_octree_write,
        [out] chunks3.VoxelNode,
        [none] chunks3.BlocksSpawned,
        [none] chunks3.VoxelNodePostDirty,
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
    zox_system(
        can_have_vodes_system,
        zoxp_update,
        [in] rendering.RenderDepth,
        [in] chunks3.ChunkLodDirty,
        [none] terrains.TerrainChunk,
    );
    zox_system_1(
        vodes_spawn_system,
        zoxp_octree_write, // zoxp_spawn,
        [in] chunks.NodeDepth,
        [in] rendering.RenderDisabled,
        [in] rendering.RenderDistance,
        [in] transforms3.Position3D,
        [out] chunks3.VoxelNode,
        [none] terrains.TerrainChunk,
        [none] vodes.CanVodes,
    );
    zox_system_1(
        BlockDamageQueueSystem,
        zoxp_spawn,
        [in] chunks3.ChunkPosition,
        [in] chunks3.VoxelNode,
        [in] chunks.NodeDepth,
        [out] blocks.BlockDamageQueue,
        [none] chunks3.Chunk3
    );
    zox_system(
        VodesSettingsDirtySystem,
        zoxp_update,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
