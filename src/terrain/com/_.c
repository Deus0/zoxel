zox_tag(Terrain);
zox_tag(TerrainLink);
zox_tag(FlatTerrain);
zox_tag(FlatlandChunk);
zox_tag(TerrainWorld);
zox_tag(TerrainChunk);
zox_tag(ChunkTerrain);
// Vegetation
zox_tag(BlockWood);
zox_tag(BlockSoil);
zox_tag(BlockSoilGrass);
zox_tag(BlockGrass);
zox_tag(BlockStone);
zox_tag(BlockSand);
zox_tag(BlockObsidian);
zox_tag(BlockBricks);
zox_tag(BlockFlower);
zox_tag(BlockRoad);
#include "spawn2.c"

entity get_linked_terrain(ecs* world, entity e) {
    return zox_get_link(world, e, TerrainLink);
}

void define_components_terrain(ecs *world) {
    zoxd_tag(Terrain);
    zoxd_nf_tag(TerrainLink);
    zoxd_tag(FlatTerrain);
    zoxd_tag(FlatlandChunk);
    zoxd_tag(TerrainWorld);
    zoxd_tag(TerrainChunk);
    zoxd_tag(ChunkTerrain);
    // Vegetation
    zoxd_tag(BlockWood);
    zoxd_tag(BlockSoil);
    zoxd_tag(BlockSoilGrass);
    zoxd_tag(BlockGrass);
    zoxd_tag(BlockStone);
    zoxd_tag(BlockSand);
    zoxd_tag(BlockObsidian);
    zoxd_tag(BlockBricks);
    zoxd_tag(BlockFlower);
    zoxd_tag(BlockRoad);
}
