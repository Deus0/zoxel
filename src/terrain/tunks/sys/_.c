#include "spawn.c"
#include "die.c"
#include "biomes.c"
#include "heights.c"
#include "vegetation.c"
#include "towns.c"
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
        Tunk2DeathSystem,
        zoxp_destroy,
        [in] voxes.VoxLink,
        [in] chunks2.Chunk2Position,
        [in] rendering.RenderDistance,
        // [in] rendering.RenderDepth,
        [none] streaming.StreamedChunk
    );

    zox_system(
        BiomeMapSystem,
        EcsPostLoad,
        [in] core.Generate,
        [in] chunks2.Chunk2Position,
        [out] tunks.BiomeMap
    );

    // TODO: Pass in BiomeMap and use biome data
    zox_system(
        HeightMapSystem,
        EcsOnUpdate,
        [in] core.Generate,
        [in] chunks2.Chunk2Position,
        [in] tunks.BiomeMap,
        [out] tunks.HeightMap
    );

    zox_system(
        VegetationMapSystem,
        EcsPreStore,
        [in] core.Generate,
        [in] chunks2.Chunk2Position,
        [in] tunks.BiomeMap,
        [out] tunks.VegetationMap
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