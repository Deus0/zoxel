zox_increment_system_with_reset(LightNodeDirty, zox_dirty_end);
#include "sunlight.c"
#include "propogate.c"
#include "queue.c"
#include "reduce.c"

void define_systems_lighting3(ecs* world) {
    zoxd_system_increment(LightNodeDirty);
    zox_system(SunlightSystem, zoxp_lights_write,
        [in] chunks3.VoxelNodeDirty,
        [in] rendering.RenderDepthDirty,
        [in] rendering.RenderDepth,
        [in] chunks3.VoxelNode,
        [out] lighting3.LightNodeDepth,
        [out] lighting3.LightNode,
        [out] lighting3.LightNodeDirty
    );

    zox_system(LightPropogateSystem, zoxp_lights_write,
        [in] lighting3.LightNodeDirty,
        [in] lighting3.LightNodeDepth,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [out] lighting3.LightNode,
        [out] lighting3.LightNodeQueue
    );

    zox_system(LightNodeReduceSystem, zoxp_lights_write,
        [in] lighting3.LightNodeDirty,
        [in] lighting3.LightNodeDepth,
        [out] lighting3.LightNode
    );

    zox_system(LightNodeQueueSystem, zoxp_lights_write + 1,
        [in] chunks3.VoxelNode,
        [in] lighting3.LightNodeDepth,
        [out] lighting3.LightNode,
        [out] lighting3.LightNodeDirty,
        [out] lighting3.LightNodeQueue
    );

}