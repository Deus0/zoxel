#include "stream_point.c"
#include "lod2.c"
#include "lod3.c"
#include "frustum.c"
#include "stream_end.c"
// #include "sync.c"

void define_systems_streaming(ecs* world) {
    zox_system(
        StreamPointSystem,
        zoxp_update,
        [in] streaming.StreamLink,
        [in] transforms3.Position3D,
        // [in] blocks.BlockScale,
        // [in] chunks.NodeDepth,
        [out] streaming.StreamPoint,
        [out] streaming.StreamPoint2,
        [out] streaming.StreamDirty,
        [out] streaming.StreamDirty2,
        [none] streaming.Streamer
    );
    // Set streamer from terrain
    /*zox_system(
        StreamPointSyncSystem,
        EcsOnUpdate,
        [in] streaming.StreamLink,
        [out] blocks.BlockScale,
        [out] chunks.NodeDepth,
        [none] streaming.Streamer
    );*/
    zox_filter(
        filter_cameras,
        [in] transforms3.Position3DBounds,
        [in] cameras.CameraPlanes,
        // [none] cameras.Camera3D
        [none] streaming.Streamer
    );
    zox_system_ctx(
        ChunkFrustumSystem,
        zoxp_voxels_read,
        filter_cameras,
        [in] transforms3.Position3D,
        [in] transforms3.Bounds3D,
        [in] chunks3.ChunkEntities,
        [in] chunks3.VoxelNode,
        [out] rendering.RenderDisabled,
        [none] streaming.StreamedChunk
    );

    zox_filter(streamers3,
        [in] streaming.StreamPoint,
        [in] streaming.StreamDirty,
        [none] streaming.Streamer
    );
    zox_system_ctx(
        ChunkLodSystem,
        zoxp_update,
        streamers3,
        [in] chunks3.ChunkPosition,
        [out] rendering.RenderDepth,
        [out] rendering.RenderDistance,
        [out] rendering.RenderDepthDirty,
        [out] rendering.RenderDistanceDirty,
        [none] streaming.StreamedChunk
    );
    zox_filter(streamers2,
        [in] streaming.StreamPoint2,
        [in] streaming.StreamDirty2,
        [none] streaming.Streamer
    );
    zox_system_ctx(
        Chunk2LodSystem,
        zoxp_update,
        streamers2,
        [in] chunks2.Chunk2Position,
        [out] rendering.RenderDistance,
        [out] rendering.RenderDepth,
        [out] rendering.RenderDistanceDirty,
        [out] rendering.RenderDepthDirty,
        [none] streaming.StreamedChunk
    );
    // streams
    // main thread
    zox_system_1(
        StreamEndEventSystem,
        zoxp_mainthread,
        [in] core.EventInput,
        [in] voxes.ChunkLinks,
        [out] StreamEndEvent
    );

    // Custom Cuttoff
    zox_set(zox_id(ChunkFrustumSystem), SystemDeltaMax, { 8 });
}
