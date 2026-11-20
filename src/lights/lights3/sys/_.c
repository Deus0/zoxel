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
#include "gpu_mesh.c"

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
        [none] !rendering.GpuMesh,
    );
    // gpu meshes (sys/gpu_mesh.c): grids filled on worker threads beside the cpu build and smooth lights, meshed on
    //  the main thread with the gpu uploads
    zox_system(
        chunk_gpu_grid_system,
        zoxp_octree_read,
        [in] rendering.RenderDepth,
        [out] rendering.GridVoxels,
        [out] rendering.GridLights,
        [out] rendering.GridPresent,
        [out] rendering.GridLightsPresent,
        [none] rendering.GpuMesh,
        [none] rendering.BuildMesh,
        [none] !core.BuildDisabled,
    );
    zox_system(
        chunk_gpu_relight_system,
        zoxp_octree_read,
        [in] rendering.RenderDepth,
        [out] rendering.GridVoxels,
        [out] rendering.GridLights,
        [out] rendering.GridPresent,
        [out] rendering.GridLightsPresent,
        [none] rendering.GpuMesh,
        [none] rendering.BuildMeshColors,
        [none] !rendering.BuildMesh,
        [none] !core.BuildDisabled,
    );
    zox_system_1(
        chunk_gpu_mesh_system,
        zoxp_gpu_upload,
        [in] rendering.RenderDepth,
        [in] rendering.MeshGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering.UvsGPULink,
        [out] rendering.GridVoxels,
        [out] rendering.GridLights,
        [in] rendering.GridPresent,
        [in] rendering.GridLightsPresent,
        [out] rendering.MeshCommandGPULink,
        [out] rendering.MeshFaceRoom,
        [out] rendering.MeshFence,
        [none] rendering.GpuMesh,
        [none] rendering.MeshDirty,
    );
    zox_system_1(
        chunk_gpu_mesh_count_system,
        zoxp_gpu_upload,
        [in] rendering.MeshGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering.UvsGPULink,
        [in] rendering.MeshCommandGPULink,
        [out] rendering.MeshFaceRoom,
        [out] rendering.MeshFence,
        [out] rendering.MeshRenderCount,
        [out] rendering.VaoGPULink,
        [none] rendering.GpuMesh,
    );
    zox_render3_system_parallel(
        1,
        chunk_gpu_render_system,
        [in] rendering.RenderDisabled,
        [in] rendering.RenderDepth,
        [in] transforms.TransformMatrix,
        [in] rendering.MeshCommandGPULink,
        [in] rendering.MeshRenderCount,
        [in] rendering.VaoGPULink,
        [none] rendering.GpuMesh,
        [none] !core.Disabled,
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
