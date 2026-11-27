#include "spawn.c"
#include "maps.c"
#include "walls.c"
#include "homes.c"
#include "roads.c"
#include "in_town.c"

void zox_systems_towns(ecs* world) {
    zox_system(
        town_map_system,
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
        town_walls_system,
        zoxp_octree_write,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkPosition,
        [out] chunks.GenerateChunk,
        [out] chunks3.VoxelNode,
        [out] ?chunks3.BlockDatas,
        [none] voxel_terrains.TerrainChunk
    );
    zox_system_1(
        region_towns_system,
        zoxp_spawn,
        [in] core.Seed,
        [in] blocks.BlockPosition2,
        [in] blocks.BlockSize2,
        [out] regions.GenerateRegion,
        [none] regions.Region
    );
    zox_system_1(
        homes_spawn_system,
        zoxp_spawn,
        [in] core.Seed,
        [in] blocks.BlockPosition2,
        [in] towns.TownSize,
        [out] towns.GenerateTown,
        [none] towns.Town
    );
    zox_system(
        town_roads_system,
        zoxp_update,
        [in] blocks.BlockPosition2,
        [in] towns.TownSize,
        [in] towns.WallThickness,
        [in] towns.TownGateSide,
        [in] towns.TownGateAt,
        [in] core.Seed,
        [out] towns.GenerateTown,
        [out] towns.RoadSegments,
        [out] towns.StreetLamps,
        [none] towns.Town
    );
    zox_system_1(
        in_town_system,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [none] players.PlayerCharacter
    );
    zox_on_add(town_left_event, [none] towns.TownLeft);
}
