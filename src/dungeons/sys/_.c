#include "expand.c"
#include "spawn.c"
#include "maps.c"
#include "place.c"

void zox_systems_dungeons(ecs* world) {
    // Spawns new blocks
    zox_system_1(
        DungeonBlockSystem,
        zoxp_spawn, // zoxp_queue_add,
        [in] timing.TimerState,
        [in] chunks3.ChunkLink,
        [in] DungeonWallType,
        [none] blocks.BlockDungeon
    );
    // A region has manyy dungeons
    zox_system_1(
        DungeonsSpawnSystem,
        zoxp_spawn,
        [in] core.Seed,
        [in] blocks.BlockPosition2,
        [in] blocks.BlockSize2,
        [out] regions.GenerateRegion,
        [none] regions.Region
    );
    // 2D layout for dungeons
    /*zox_system(
        DungeonMapSystem,
        zoxp_update,
        [in] tunks.TunkLod,
        [in] tunks.TunkPosition,
        [in] tunks.BiomeMap,
        [out] tunks.GenerateTunk,
        [out] tunks.HeightMap,
        [out] vegetation.VegetationMap,
        [out] towns.TownMap,
        [none] tunks.Tunk
    );
    // NOTE: Before vegetation atm
    zox_system(
        TownWallsSystem,
        zoxp_update,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkPosition,
        [out] chunks.GenerateChunk,
        [out] chunks3.VoxelNode,
        [none] terrains.TerrainChunk
    );*/
}
