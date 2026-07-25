#include "maps.c"
#include "chunks.c"

void zox_define_systems_vegetation(ecs* world) {
    // NOTE: Vegetation maps need biomes and temperature maps
    zox_system(
        VegetationMapSystem,
        zoxp_update,
        [in] tunks.TunkLod,
        [in] tunks.TunkPosition,
        [in] tunks.BiomeMap,
        [out] tunks.GenerateTunk,
        [out] vegetation.VegetationMap,
        [none] tunks.Tunk
    );
    zox_system(
        VegetationChunk3System,
        zoxp_update,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkPosition,
        [in] tunks.TunkLink,
        [out] chunks.GenerateChunk,
        [out] chunks3.VoxelNode,
        [none] terrains.TerrainChunk
    );
    add_system_process_counter(world, zox_id(VegetationChunk3System));
}