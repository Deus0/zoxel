#include "decayed.c"
#include "soil_blended.c"
#include "soil.c"
#include "road.c"
#include "sand.c"
#include "bricks.c"
#include "flowers.c"
#include "rubble.c"
#include "wood.c"
#include "noise.c"

void zox_systems_models_generation(ecs* world) {
    zox_system(
        noise_model_system,
        zoxp_octree_write,
        [in] chunks.NodeDepth,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeLock,
        [out] colorz.ColorRGBs,
        [none] chunks3.NoiseChunk
    );
    zox_system(
        decayed_model_system,
        zoxp_octree_write,
        [in] core.Seed,
        [in] colorz.Color,
        [in] models.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
    zox_system(
        blended_soil_generation_system,
        zoxp_octree_write,
        [in] core.Seed,
        [in] colorz.Color,
        [in] models.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
    zox_system(
        soil_model_system,
        zoxp_octree_write,
        [in] core.Seed,
        [in] colorz.Color,
        [in] models.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
    zox_system(
        road_model_generation_system,
        zoxp_octree_write,
        [in] core.Seed,
        [in] colorz.Color,
        [in] models.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
    zox_system(
        sand_model_generation_system,
        zoxp_octree_write,
        [in] core.Seed,
        [in] colorz.Color,
        [in] models.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
    zox_system(
        bricks_model_generation_system,
        zoxp_octree_write,
        [in] core.Seed,
        [in] colorz.Color,
        [in] models.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
    zox_system(
        flowers_model_generation_system,
        zoxp_octree_write,
        [in] core.Seed,
        [in] colorz.Color,
        [in] models.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
    zox_system(
        rubble_model_system,
        zoxp_octree_write,
        [in] core.Seed,
        [in] colorz.Color,
        [in] models.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
    zox_system(
        wood_model_generation_system,
        zoxp_octree_write,
        [in] core.Seed,
        [in] colorz.Color,
        [in] models.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
}
