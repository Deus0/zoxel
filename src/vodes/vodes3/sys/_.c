#include "removed.c"
#include "despawn.c"
#include "spawn.c"
#include "lods.c"
#include "block_damage.c"
#include "block_health_overlay.c"
#include "can.c"
#include "settings.c"
#include "lights.c"
#include "animate.c"

void zox_systems_vodes3(ecs* world) {
    zox_system(
        vode_animate_system,
        zoxp_update,
        [in] blocks.VoxelLocalPosition,
        [in] rendering.RenderDepth,
        [in] blocks.BlockIndex,
        [none] models.ModelAnimated,
        [none] rendering.RendererInstance,
    );
    // NOTE: Writes to VoxelNode
    zox_system(
        vodes_despawn_system,
        zoxp_octree_write,
        [in] rendering.RenderDistanceDirty,
        [in] rendering.RenderDepth,
        [out] chunks3.VoxelNode,
        [none] chunks3.BlocksSpawned,
        [none] !vodes.CanVodes,
    );
    // NOTE: Writes to VoxelNode
    zox_system(
        vodes_remove_system,
        zoxp_octree_write,
        [out] chunks3.VoxelNode,
        [none] chunks3.BlocksSpawned,
        [none] chunks3.VoxelNodePostDirty,
    );
    zox_system(
        vodes_lod_system,
        zoxp_update,
        [in] rendering.RenderDistanceDirty,
        [in] rendering.RenderDistance,
        [in] chunks3.VoxelNode,
        [none] chunks3.BlocksSpawned,
    );
    zox_system(
        block_health_overlay_system,
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
        [none] voxel_terrains.TerrainChunk,
    );
    zox_system_1(
        vodes_spawn_system,
        zoxp_spawn, // zoxp_octree_write, // zoxp_spawn,
        [in] chunks.NodeDepth,
        [in] rendering.RenderDisabled,
        [in] rendering.RenderDistance,
        [in] transforms3.Position3D,
        [in] chunks3.ChunkPosition,
        [out] chunks3.VoxelNode,
        [none] voxel_terrains.TerrainChunk,
        [none] vodes.CanVodes,
    );
    zox_system_1(
        block_damage_queue_system,
        zoxp_spawn,
        [in] chunks3.ChunkPosition,
        [in] chunks3.VoxelNode,
        [in] chunks.NodeDepth,
        [out] blocks.BlockDamageQueue,
        [none] chunks3.Chunk3
    );
    zox_system(
        block_vox_lights_system,
        zoxp_update,
        [in] lights3.LightOctreeDirty,
        [in] chunks3.ChunkNeighbors,
        [none] voxel_terrains.TerrainChunk,
    );
    // its settings: spawned on the app, applied when one changes (needs prefab_app)
    add_hook_spawn_prefabs(vodes_settings_events);
}
