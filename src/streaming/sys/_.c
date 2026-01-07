#include "stream_point.c"
#include "stream_updates.c"
#include "frustum.c"
#include "stream_end.c"
#include "spawn.c"
#include "death.c"

void define_systems_streaming(ecs* world) {
    zox_system(
        StreamPointSystem,
        zoxp_update,
        [in] transforms3.Position3D,
        [in] terrain.TerrainLink,
        [out] StreamPoint,
        [out] StreamDirty,
        [none] Streamer
    );
    zox_filter(
        filter_cameras,
        [in] generic.Position3DBounds,
        [in] cameras.CameraPlanes,
        [none] cameras.Camera3D
    );
    zox_system_ctx(
        ChunkFrustumSystem,
        zoxp_voxels_read,
        filter_cameras,
        [in] transforms3.Position3D,
        [in] generic.Bounds3D,
        [in] chunks3.ChunkEntities,
        [in] chunks3.VoxelNode,
        [out] rendering.RenderDisabled,
        [none] StreamedChunk
    );
    zox_filter(streamers2,
        [in] StreamPoint,
        [in] StreamDirty);
    zox_system_ctx(
        ChunkLodSystem,
        zoxp_update,
        streamers2,
        [in] chunks3.ChunkPosition,
        [out] rendering.RenderDepth,
        [out] rendering.RenderDepthDirty,
        [out] rendering.RenderDistance,
        [out] rendering.RenderDistanceDirty,
        [none] StreamedChunk
    );
    // streams
    zox_system(
        ChunkDieSystem,
        zoxp_destroy,
        [in] voxes.VoxLink,
        [in] chunks3.ChunkPosition,
        [in] rendering.RenderDistance,
        [in] rendering.RenderDepth,
        [none] StreamedChunk
    );
    // main thread
    zox_system_1(
        StreamEndEventSystem,
        zoxp_mainthread,
        [in] generic.EventInput,
        [in] voxes.ChunkLinks,
        [out] StreamEndEvent
    );
    zox_filter(
        streamers,
        [in] StreamPoint
    );
    zox_system_ctx_1(
        ChunkSpawnSystem,
        zoxp_mainthread,
        streamers,
        [in] chunks3.ChunkPosition,
        [in] voxes.VoxLink,
        [in] rendering.RenderDistance,
        [out] chunks3.ChunkNeighbors,
        [none] StreamedChunk
    );
}
