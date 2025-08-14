zox_increment_system_with_reset(LightNodeDirty, zox_dirty_end);
#include "sunlight.c"
#include "queue.c"

void define_systems_lighting3(ecs* world) {
    zoxd_system_increment(LightNodeDirty);
    zox_system_1(SunlightSystem, zoxp_lights_write,
        [in] rendering.RenderDepthDirty,
        [in] rendering.RenderDepth,
        [in] chunks3.VoxelNode,
        [out] lighting3.LightNodeDepth,
        [out] lighting3.LightNode,
        [out] lighting3.LightNodeDirty
    );
    zox_system(LightNodeQueueSystem, zoxp_lights_write,
        [in] chunks3.VoxelNode,
        [in] lighting3.LightNodeDepth,
        [out] lighting3.LightNodeQueue,
        [out] lighting3.LightNode,
        [out] lighting3.LightNodeDirty
    );
}