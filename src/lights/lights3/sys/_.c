#include "editing.c"
#include "sunlight.c"
#include "lightbeam.c"
#include "flood.c"
#include "dark.c"
#include "reduce.c"
#include "build.c"
#include "build_smooth.c"
#include "trigger.c"
#include "trigger_mesh.c"

// Toggle the light systems
void set_light_systems(ecs* world, byte is_lights) {
    zox_set_enabled(SunlightSystem, is_lights);
    zox_set_enabled(LightBeamSystem, is_lights);
    zox_set_enabled(LightFloodSystem, is_lights);
    zox_set_enabled(DarkLightSystem, is_lights);
    zox_set_enabled(VoxelLightSystem, is_lights);
    zox_set_enabled(ChunkColorsTriggerSystem, is_lights);
    zox_set_enabled(ChunkNeighborLightTriggerSystem, is_lights);
    zox_set_enabled(SmoothLightsBuildSystem, is_lights);
}

#include "settings.c"
// TODO: Rename NodeDepth to OctreeDepth

void define_systems_lights3(ecs* world) {
    // Settings
    zox_system_1(
        LightsSettingsSystem,
        zoxp_spawn,
        [in] settings.LoadSettings
    );
    zox_system(
        LightsSettingsDirtySystem,
        zoxp_update,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
    zox_system(
        SunlightSystem,
        zoxp_update,
        [in] chunks.NodeDepth,
        [out] lights3.SunlightQueue,
        [none] lights.GenerateSunlight,
        [none] lights3.SunnyChunk,
        [none] chunks.Chunk,
    );
    zox_system(
        LightBeamSystem,
        zoxp_update,
        [in] blocks.BlockManagerLink,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkNeighbors,
        [out] lights3.LightNodeLock,
        [out] lights3.SunlightQueue,
        [out] lights3.LightQueue,
        [out] lights3.LightNode,
        [out] lights3.LightNodeDirty,
        [none] chunks.Chunk,
        [none] !chunks.GenerateChunk,
    );
    zox_system(
        LightFloodSystem,
        zoxp_update,
        [in] blocks.BlockManagerLink,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkNeighbors,
        [out] lights3.LightNodeLock,
        [out] lights3.LightQueue,
        [out] lights3.LightNode,
        [out] lights3.LightNodeDirty,
        [none] chunks.Chunk,
        [none] !chunks.GenerateChunk,
    );
    zox_system(
        DarkLightSystem,
        zoxp_update,
        [in] blocks.BlockManagerLink,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [out] lights3.LightNodeLock,
        [out] lights3.LightNodeDepth,
        [out] lights3.LightNode,
        [out] lights3.DarkQueue,
        [out] lights3.LightQueue,
        [out] lights3.LightNodeDirty,
        [none] chunks.Chunk,
        [none] !chunks.GenerateChunk,
    );
    // NOTE: This needs to be queue dependent
    zox_system(
        VoxelLightSystem,
        zoxp_queue_process,
        [in] blocks.BlockManagerLink,
        [in] chunks3.VoxelNodeQueue,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkNeighbors,
        [out] lights3.LightNodeLock,
        [out] lights3.SunlightQueue,
        [out] lights3.LightQueue,
        [out] lights3.DarkQueue,
        [out] lights3.LightNode,
        [out] lights3.LightNodeDirty,
        [none] chunks3.Chunk3,
        [none] !chunks.GenerateChunk,
    );
    // this kinda has issues atm hmm
    zox_system(
        LightNodeReduceSystem,
        zoxp_update,
        [in] lights3.LightNodeDirty,
        [out] lights3.LightNodeLock,
        [out] lights3.LightNode,
        [none] chunks.Chunk
    );
    zox_system(
        ChunkColorsTriggerSystem,
        zoxp_update,
        [in] lights3.LightNodeDirty,
        [none] chunks.Chunk,
    );
    zox_system(
        ChunkNeighborLightTriggerSystem,
        zoxp_update,
        [in] lights3.LightNodeDirty,
        [in] chunks3.ChunkNeighbors,
        [none] chunks.Chunk
    );
    zox_system_1(
        LightNodeDebugSystem,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [in] blocks.BlockScale,
        [in] rendering.RenderDistance,
        [in] chunks3.ChunkPosition,
        [in] chunks3.VoxelNode,
        [in] lights3.LightNode,
        [in] rendering.RenderDepth,
        [none] chunks.Chunk,
    );
    // Mesh
    // Wait until after mesh has built
    zox_system(
        SmoothLightsBuildSystem,
        zoxp_voxels_lights,
        [in] rendering.RenderDepth,
        [in] rendering.MeshColorRGBs,
        [none] chunks.ChunkMesh,
        [none] rendering.BuildMeshColors,
        [none] !core.BuildDisabled,
        [none] !rendering.MeshDirty,
        [none] !rendering.BuildMesh,
    );

    /*zox_system(
        BasicLightsBuildSystem,
        zoxp_voxels_lights,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [in] chunks3.SidesOctree,
        [in] lights3.LightNode,
        [in] rendering.RenderDepth,
        [in] rendering.MeshColorRGBs,
        [out] rendering.MeshReady,
        [none] rendering.BuildMeshColors,
        [none] chunks.Chunk,
        [none] !rendering.BuildMesh,
        [none] !core.BuildDisabled,
    );*/
}

/*zox_system(
    RefreshLightsSystem,
    zoxp_update,
    [in] blocks.BlockManagerLink,
    [in] chunks3.VoxelNode,
    [in] chunks3.ChunkNeighbors,
    [out] lights3.RefreshLights,
    [out] lights3.LightNode,
    [out] lights3.LightNodeDirty,
    [none] chunks.Chunk,
);*/
