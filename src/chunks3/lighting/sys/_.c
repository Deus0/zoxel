zox_increment_system_with_reset(LightNodeDirty, zox_dirty_end);
zox_increment_system_with_reset(SunlightDirty, zox_dirty_end);
#include "editing.c"
#include "light_start.c"
#include "light.c"
#include "light_propogate.c"
#include "light_flood.c"
#include "darklight.c"
#include "reduce.c"
#include "trigger.c"
#include "builder.c"

void define_systems_lighting3(ecs* world) {
    zoxd_system_increment(LightNodeDirty);
    zoxd_system_increment(SunlightDirty);

    zox_system(
        SunlightBatchSystem,
        zoxp_lights_write,
        [in] chunks3.VoxelNodeGenerated,
        [in] rendering.RenderDepth,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkNeighbors,
        [out] lighting3.LightNodeDepth,
        [out] lighting3.LightNode,
        [out] lighting3.SunlightDirty,
        [none] lighting3.SunnyChunk
    );

    zox_system(LightPropogateBatchSystem,
        zoxp_lights_write,
        [in] lighting3.SunlightDirty,
        [in] lighting3.LightNodeDepth,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [out] lighting3.LightNode,
        [out] lighting3.LightNodeDirty
    );

    zox_system(VoxelLightSystem,
        zoxp_queue_process2,
        [in] chunks3.VoxelNodeQueue,
        [in] rendering.RenderDepth,
        [in] lighting3.LightNode,
        [in] chunks3.ChunkNeighbors,
        [out] lighting3.SunlightQueue,
        [out] lighting3.PropogateQueue,
        [out] lighting3.DarkQueue
    );

    zox_system(
        SunlightQueueSystem,
        zoxp_lights_write,
        [in] rendering.RenderDepth,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkNeighbors,
        [out] lighting3.SunlightQueue,
        [out] lighting3.LightNodeDepth,
        [out] lighting3.LightNode,
        [out] lighting3.SunlightDirty,
        [out] lighting3.PropogateQueue
    );

    zox_system(DarkLightSystem,
        zoxp_lights_write + 1,
        [in] lighting3.LightNodeDepth,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [out] lighting3.LightNode,
        [out] lighting3.DarkQueue,
        [out] lighting3.PropogateQueue,
        [out] lighting3.LightNodeDirty
    );

    zox_system(LightFloodSystem,
        zoxp_lights_write + 2,
        [in] lighting3.LightNodeDepth,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [in] lighting3.DarkQueue,
        [out] lighting3.LightNode,
        [out] lighting3.PropogateQueue,
        [out] lighting3.LightNodeDirty
    );

    zox_system(MeshColorsTriggerSystem,
        zoxp_lights_write + 3,
        [in] lighting3.SunlightQueue,
        [in] lighting3.PropogateQueue,
        [in] chunks3.ChunkMeshDirty,
        [in] lighting3.SunlightDirty,
        [in] lighting3.LightNodeDirty,
        [out] rendering.MeshColorsGenerate
    );

    zox_system(Light3BuildSystem,
        zoxp_voxels_read + 1,
        [in] rendering.MeshColorsGenerate,
        [in] chunks3.VoxLink,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [in] lighting3.LightNode,
        [in] rendering.RenderDepth,
        [in] rendering.MeshColorRGBs,
        [out] rendering.MeshColorsDirty
    );

    zox_system_1(
        LightNodeDebugSystem,
        EcsOnUpdate,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
        [in] rendering.RenderDistance,
        [in] chunks3.ChunkPosition,
        [in] chunks3.VoxelNode,
        [in] lighting3.LightNode,
        [in] lighting3.LightNodeDepth
    );

    // this kinda has issues atm hmm
    zox_system(LightNodeReduceSystem,
        zoxp_lights_write,
        [in] lighting3.LightNodeDirty,
        [in] lighting3.LightNodeDepth,
        [out] lighting3.LightNode
    );
}