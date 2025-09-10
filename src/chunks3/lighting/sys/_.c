#include "editing.c"
#include "light_start.c"
#include "light.c"
#include "darkness.c"
#include "reduce.c"
#include "trigger.c"
#include "builder.c"

// TODO: Rename NodeDepth to VoxelNodeDepth
// TODO: Rename all NodeDepth to OctreeDepth

void define_systems_lighting3(ecs* world) {
    zox_system(
        SunlightSystem,
        zoxp_lights_write,
        [in] chunks3.VoxelNodeGenerated,
        [in] chunks3.NodeDepth,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxLink,
        [out] lighting3.LightNodeDepth,
        [out] lighting3.LightNode,
        [out] lighting3.LightNodeDirty,
        [none] lighting3.SunnyChunk
    );

    zox_system(
        LightSystem,
        zoxp_lights_write,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxLink,
        // [out] lighting3.LightNodeDepth,
        [out] lighting3.LightNode,
        [out] lighting3.LightQueue,
        [out] lighting3.LightNodeDirty
    );

    zox_system(
        DarkLightSystem,
        zoxp_lights_write,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [in] chunks3.VoxLink,
        [out] lighting3.LightNodeDepth,
        [out] lighting3.LightNode,
        [out] lighting3.DarkQueue,
        [out] lighting3.LightQueue,
        [out] lighting3.LightNodeDirty
    );

    // NOTE: This needs to be queue dependent
    zox_system(
        VoxelLightSystem,
        zoxp_queue_process2,
        [in] chunks3.VoxelNodeQueue,
        [in] chunks3.NodeDepth,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxLink,
        [out] lighting3.LightNode,
        [out] lighting3.LightQueue,
        [out] lighting3.DarkQueue,
        [out] lighting3.LightNodeDirty
    );

    // this kinda has issues atm hmm
    zox_system(
        LightNodeReduceSystem,
        zoxp_lights_write + 1,
        [in] lighting3.LightNodeDirty,
        [out] lighting3.LightNode
    );

    zox_system(
        MeshColorsTriggerSystem,
        zoxp_lights_write + 2,
        [in] lighting3.LightQueue,
        [in] lighting3.DarkQueue,
        [in] chunks3.ChunkMeshDirty,
        [in] lighting3.SunlightDirty,
        [in] lighting3.LightNodeDirty,
        [out] rendering.MeshColorsGenerate
    );

    zox_system(
        Light3BuildSystem,
        zoxp_voxels_read + 2,
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
        zoxp_mainthread,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
        [in] rendering.RenderDistance,
        [in] chunks3.ChunkPosition,
        [in] chunks3.VoxelNode,
        [in] lighting3.LightNode,
        [in] rendering.RenderDepth
    );
}