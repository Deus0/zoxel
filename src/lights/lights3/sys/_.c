#include "editing.c"
#include "sunlight.c"
#include "flood.c"
#include "darkness.c"
#include "reduce.c"
#include "trigger.c"
#include "build.c"

// TODO: Rename NodeDepth to VoxelNodeDepth
// TODO: Rename all NodeDepth to OctreeDepth

void define_systems_lights3(ecs* world) {
    zox_system(
        SunlightSystem,
        zoxp_lights_write,
        [in] blocks.BlockManagerLink,
        [in] lights.GenerateLights,
        [in] chunks.NodeDepth,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkNeighbors,
        [out] lights3.LightQueue,
        [out] lights3.LightNodeDepth,
        [out] lights3.LightNode,
        [out] lights3.LightNodeDirty,
        [none] lights3.SunnyChunk,
        [none] chunks.Chunk
    );
    zox_system(
        LightBeamSystem,
        zoxp_lights_write,
        [in] blocks.BlockManagerLink,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkNeighbors,
        [out] lights3.SunlightQueue,
        [out] lights3.LightQueue,
        [out] lights3.LightNode,
        [out] lights3.LightNodeDirty,
        [none] chunks.Chunk
    );
    zox_system(
        LightFloodSystem,
        zoxp_lights_write,
        [in] blocks.BlockManagerLink,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkNeighbors,
        [out] lights3.LightQueue,
        [out] lights3.LightNode,
        [out] lights3.LightNodeDirty,
        [none] chunks.Chunk
    );
    zox_system(
        DarkLightSystem,
        zoxp_lights_write,
        [in] blocks.BlockManagerLink,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [out] lights3.LightNodeDepth,
        [out] lights3.LightNode,
        [out] lights3.DarkQueue,
        [out] lights3.LightQueue,
        [out] lights3.LightNodeDirty,
        [none] chunks.Chunk
    );
    // NOTE: This needs to be queue dependent
    zox_system(
        VoxelLightSystem,
        zoxp_queue_process2,
        [in] blocks.BlockManagerLink,
        [in] chunks3.VoxelNodeQueue,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkNeighbors,
        [out] lights3.SunlightQueue,
        [out] lights3.LightQueue,
        [out] lights3.DarkQueue,
        [out] lights3.LightNode,
        [out] lights3.LightNodeDirty,
        [none] chunks.Chunk
    );
    // this kinda has issues atm hmm
    zox_system(
        LightNodeReduceSystem,
        zoxp_lights_write,
        [in] lights3.LightNodeDirty,
        [out] lights3.LightNode,
        [none] chunks.Chunk
    );
    zox_system(
        MeshColorsTriggerSystem,
        zoxp_lights_write + 2,
        [in] lights3.LightQueue,
        [in] lights3.DarkQueue,
        [in] chunks3.VoxelNodeDirty,
        [in] chunks3.ChunkMeshDirty,
        [in] lights3.LightNodeDirty,
        [out] rendering.MeshColorsGenerate,
        [none] chunks.Chunk
    );
    zox_system(
        Light3BuildSystem,
        zoxp_voxels_read + 2,
        [in] chunks3.VoxelNodeDirty,
        [in] rendering.MeshColorsGenerate,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [in] chunks3.SidesOctree,
        [in] lights3.LightNode,
        [in] rendering.RenderDepth,
        [in] rendering.MeshColorRGBs,
        [out] rendering.MeshColorsDirty,
        [none] chunks.Chunk
    );
    zox_system_1(
        LightNodeDebugSystem,
        zoxp_mainthread,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
        [in] rendering.RenderDistance,
        [in] chunks3.ChunkPosition,
        [in] chunks3.VoxelNode,
        [in] lights3.LightNode,
        [in] rendering.RenderDepth,
        [none] chunks.Chunk
    );
}
