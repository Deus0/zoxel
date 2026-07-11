#include "spawn.c"
#include "maps.c"
#include "walls.c"

void zox_define_systems_towns(ecs* world) {
    zox_system(
        TownMapSystem,
        zoxp_update,
        [in] regions.RegionLink,
        [in] tunks.TunkPosition,
        [in] tunks.BiomeMap,
        [out] tunks.GenerateTunk,
        [out] tunks.HeightMap,
        [out] tunks.VegetationMap,
        [out] towns.TownMap,
        [none] tunks.Tunk
    );
    // NOTE: Before vegetation atm
    zox_system(
        TownWallsSystem,
        zoxp_voxels_write,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkPosition,
        [in] tunks.TunkLink,
        [out] chunks3.GenerateChunk,
        [out] chunks3.VoxelNode,
        [none] terrains.TerrainChunk
    );
    zox_system_1(
        RegionTownsSystem,
        zoxp_mainthread,
        [in] core.Seed,
        [in] blocks.BlockPosition2,
        [in] blocks.BlockSize2,
        [out] regions.GenerateRegion,
        [none] regions.Region
    );
}
