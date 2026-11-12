#include "maps.c"
#include "chunks.c"

void zox_systems_vegetation(ecs* world) {
    // NOTE: Vegetation maps need biomes and temperature maps
    zox_system(
        vegetation_map_system,
        zoxp_update,
        [in] tunks.TunkLod,
        [in] tunks.TunkPosition,
        [in] tunks.BiomeMap,
        [out] tunks.GenerateTunk,
        [out] vegetation.VegetationMap,
        [none] tunks.Tunk
    );
    zox_system(
        vegetation_chunk3_system,
        zoxp_octree_write,
        [in] chunks.NodeDepth,
        [in] chunks3.ChunkPosition,
        [out] chunks.GenerateChunk,
        [out] chunks3.VoxelNode,
        [none] terrains.TerrainChunk
    );
    add_system_process_counter(world, zox_id(vegetation_chunk3_system));
}
