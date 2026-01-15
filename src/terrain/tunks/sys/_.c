#include "spawn.c"
#include "heights.c"
#include "end.c"

void define_systems_tunks(ecs* world) {
    zox_filter(
        streamers,
        [in] streaming.StreamPoint2,
        [none] streaming.Streamer
    );
    zox_system_ctx_1(
        Tunk2SpawnSystem,
        zoxp_mainthread,
        streamers,
        [in] voxes.VoxLink,
        [in] chunks2.Chunk2Position,
        [in] rendering.RenderDistance,
        [out] chunks2.Chunk2Neighbors,
        [none] streaming.StreamedChunk
    );

    zox_system(
        HeightMapSystem,
        EcsOnUpdate,
        [in] core.Generate,
        [in] chunks2.Chunk2Position,
        [out] tunks.HeightMap
    );

    zox_system(
        TunkEndSystem,
        EcsOnUpdate,
        [in] core.Generate,
        [in] chunks2.Chunk2Position,
        [in] voxes.VoxLink
    );


    /*zox_system_1(
        TunkDebugSystem,
        [in] voxes.VoxLink,
        [in] chunks2.Chunk2Position,
        [in] rendering.RenderDistance,
        [out] chunks2.Chunk2Neighbors,
    );*/
}