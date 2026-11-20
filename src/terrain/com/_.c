zox_tag(Terrain);
zox_tag(TunkUnloading);     // a tunk the stream left, deleted a little a frame (sys/stream_unload.c), kept if it comes back
zox_tag(TerrainLink);
zox_tag(FlatTerrain);
zox_tag(FlatlandChunk);
zox_tag(TerrainWorld);
zox_tag(TerrainChunk);
zox_tag(ChunkTerrain);
// Vegetation
zox_tag(BlockWood);
zox_tag(BlockLeaves);      // a tree's canopy (vegetation/sys/chunks.c)
zox_tag(BlockBed);         // 2 x 1 x 1 blocks (BlockSize), doc/plans/multi_block_vox.md
zox_tag(BlockDoor);        // 1 x 2 x 1 blocks
zox_tag(BlockFiller);      // the rest of a bed or door: solid, drawn by the base (items/fun/multi_block.c)
zox_tag(BlockGlowStone);   // glows (BlockLight), on homes' roofs (towns/sys/walls.c)
zox_tag(BlockSoil);
zox_tag(BlockSoilGrass);
zox_tag(BlockGrass);
zox_tag(BlockStone);
zox_tag(BlockSand);
zox_tag(BlockObsidian);
zox_tag(BlockBricks);
zox_tag(BlockFlower);
zox_tag(BlockRoad);
zox_tag(BlockWater);       // a liquid (BlockMaterial): see through, walked through, it flows (terrain/water)
zox_tag(BlockWaterFlowing); // water spread from a source, a level of it (BlockWaterLevel 1 to 7): it drains
// water flowing on a terrain (sys/water_flow.c): a block to check when its time comes
typedef struct {
    int3 block;
    double at;
} WaterFlow;
zoxc_arrayd(WaterFlows, WaterFlow)
zoxc_byte(InsideBlockLast);
zox_tag(InWater);           // its lower body is in water (InsideBlock, collisions/eve/water_splash.c): it swims
#include "spawn2.c"

entity get_linked_terrain(ecs* world, entity e) {
    return zox_get_link(world, e, TerrainLink);
}

void zox_components_terrain(ecs *world) {
    zoxd_tag(Terrain);
    zoxd_tag(TunkUnloading);
    zoxd_nf_link(TerrainLink);
    zoxd_tag(FlatTerrain);
    zoxd_tag(FlatlandChunk);
    zoxd_tag(TerrainWorld);
    zoxd_tag(TerrainChunk);
    zoxd_tag(ChunkTerrain);
    // Vegetation
    zoxd_tag(BlockWood);
    zoxd_tag(BlockLeaves);
    zoxd_tag(BlockBed);
    zoxd_tag(BlockDoor);
    zoxd_tag(BlockFiller);
    zoxd_tag(BlockGlowStone);
    zoxd_tag(BlockSoil);
    zoxd_tag(BlockSoilGrass);
    zoxd_tag(BlockGrass);
    zoxd_tag(BlockStone);
    zoxd_tag(BlockSand);
    zoxd_tag(BlockObsidian);
    zoxd_tag(BlockBricks);
    zoxd_tag(BlockFlower);
    zoxd_tag(BlockRoad);
    zoxd_tag(BlockWater);
    zoxd_tag(BlockWaterFlowing);
    zoxd_arrayd(WaterFlows);
    zoxd_byte(InsideBlockLast);
    zoxd_tag(InWater);
}
