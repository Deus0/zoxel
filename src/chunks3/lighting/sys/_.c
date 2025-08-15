zox_increment_system_with_reset(LightNodeDirty, zox_dirty_end);
#include "sunlight.c"
#include "sunlight_queue.c"
#include "propogate.c"
#include "propogate_queue.c"
#include "reduce.c"
#include "trigger.c"
#include "light_voxel_queue.c"

// TODO: Light Depth Set System
// TODO: Light Clear System

void define_systems_lighting3(ecs* world) {
    zoxd_system_increment(LightNodeDirty);

    zox_system(
        SunlightSystem,
        zoxp_lights_write,

        [in] rendering.RenderDepth,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNodeGenerated,

        [out] lighting3.LightNodeDepth,
        [out] lighting3.LightNode,
        [out] lighting3.LightNodeDirty,
        [out] rendering.MeshColorsGenerate,

        [none] lighting3.SunnyChunk
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
        [out] lighting3.LightNodeDirty,
        [out] rendering.MeshColorsGenerate
    );

    /*zox_system(LightPropogateSystem,
        zoxp_lights_write,
        [in] lighting3.LightNodeDirty,
        [in] lighting3.LightNodeDepth,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [out] lighting3.LightNode,
        [out] lighting3.LightNodeQueue
    );

    zox_system(PropogateQueueSystem,
        zoxp_lights_write + 1,
        [in] chunks3.VoxelNode,
        [in] lighting3.LightNodeDepth,
        [out] lighting3.LightNode,
        [out] lighting3.LightNodeDirty,
        [out] lighting3.LightNodeQueue
    );*/

    zox_system(VoxelLightSystem,
        zoxp_queue_process2,
        [in] chunks3.VoxelNodeQueue,
        [in] rendering.RenderDepth,
        [in] lighting3.LightNode,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkNeighbors,
        [out] lighting3.SunlightQueue
    );

    // this kinda has issues atm hmm
    /*zox_system(LightNodeReduceSystem,
        zoxp_lights_write,
        [in] lighting3.LightNodeDirty,
        [in] lighting3.LightNodeDepth,
        [out] lighting3.LightNode
    );*/

    zox_system(MeshColorsTriggerSystem,
        EcsOnUpdate,
        [in] chunks3.ChunkMeshDirty,
        [out] rendering.MeshColorsGenerate
    );

}