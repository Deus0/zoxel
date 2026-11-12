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
    zox_set_enabled(sunlight_system, is_lights);
    zox_set_enabled(light_beam_system, is_lights);
    zox_set_enabled(light_flood_system, is_lights);
    zox_set_enabled(dark_light_system, is_lights);
    zox_set_enabled(voxel_light_system, is_lights);
    zox_set_enabled(chunk_colors_trigger_system, is_lights);
    zox_set_enabled(chunk_neighbor_light_trigger_system, is_lights);
    zox_set_enabled(build_smooth_lights_system, is_lights);
    zox_set_enabled(LightNodeDirtyIncrementSystem, is_lights);
    zox_set_enabled(light_node_reduce_system, is_lights);
}

// TODO: Rename NodeDepth to OctreeDepth

void zox_systems_lights3(ecs* world) {
    zox_system(
        sunlight_system,
        zoxp_update,
        [in] chunks.NodeDepth,
        [out] lights3.SunlightQueue,
        [none] chunks3.Chunk3,
        [none] lights3.SunnyChunk,
        [none] lights.GenerateSunlight,
    );
    zox_system(
        light_beam_system,
        zoxp_octree_write,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkNeighbors,
        [out] lights.LightLock,
        [out] lights3.SunlightQueue,
        [out] lights3.LightQueue,
        [out] lights3.LightNode,
        [out] lights3.LightNodeDirty,
        [none] chunks3.Chunk3,
        // [none] !chunks.GenerateChunk,
    );
    zox_system(
        light_flood_system,
        zoxp_octree_write,
        [in] chunks3.VoxelNode,
        [in] chunks3.ChunkNeighbors,
        [out] lights.LightLock,
        [out] lights3.LightQueue,
        [out] lights3.LightNode,
        [out] lights3.LightNodeDirty,
        [none] chunks3.Chunk3,
        // [none] !chunks.GenerateChunk,
    );
    zox_system(
        dark_light_system,
        zoxp_update,
        [in] chunks3.ChunkNeighbors,
        [in] chunks3.VoxelNode,
        [out] lights.LightLock,
        [out] lights3.LightNodeDepth,
        [out] lights3.LightNode,
        [out] lights3.DarkQueue,
        [out] lights3.LightQueue,
        [out] lights3.LightNodeDirty,
        [none] chunks3.Chunk3,
        // [none] !chunks.GenerateChunk,
    );
    // NOTE: This needs to be queue dependent
    zox_system(
        voxel_light_system,
        zoxp_queue_process, // zoxp_octree_write
        [in] chunks3.VoxelNodeQueue,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkNeighbors,
        [out] lights.LightLock,
        [out] lights3.SunlightQueue,
        [out] lights3.LightQueue,
        [out] lights3.DarkQueue,
        [out] lights3.LightNode,
        [out] lights3.LightNodeDirty,
        [none] chunks3.Chunk3,
        // [none] !chunks.GenerateChunk,
    );
    // this kinda has issues atm hmm
    zox_system(
        light_node_reduce_system,
        zoxp_octree_write,
        [in] lights3.LightNodeDirty,
        [out] lights.LightLock,
        [out] lights3.LightNode,
        [none] chunks3.Chunk3,
    );
    zox_system(
        chunk_colors_trigger_system,
        zoxp_update,
        [in] lights3.LightNodeDirty,
        [none] chunks3.Chunk3,
    );
    zox_system(
        chunk_neighbor_light_trigger_system,
        zoxp_update,
        [in] lights3.LightNodeDirty,
        [in] chunks3.ChunkNeighbors,
        [none] chunks3.Chunk3,
    );
    // Mesh
    // Wait until after mesh has built
    // NOTE: Runs before MeshDirty upload, geometry upload waits for these colors
    zox_system(
        build_smooth_lights_system,
        zoxp_octree_read,
        [in] rendering.RenderDepth,
        [in] rendering.MeshColorRGBs,
        [none] rendering.ChunkMesh,
        [none] rendering.BuildMeshColors,
        [none] !core.BuildDisabled,
        [none] !rendering.BuildMesh,
    );
    /*zox_system(
        basic_lights_build_system,
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
    refresh_lights_system,
    zoxp_update,
    [in] chunks3.VoxelNode,
    [in] chunks3.ChunkNeighbors,
    [out] lights3.RefreshLights,
    [out] lights3.LightNode,
    [out] lights3.LightNodeDirty,
    [none] chunks.Chunk,
);*/
