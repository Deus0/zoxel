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

void define_systems_models_generation(ecs* world) {
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
        BlendedSoilGenerationSystem,
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
        RoadModelGenerationSystem,
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
        SandModelGenerationSystem,
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
        BricksModelGenerationSystem,
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
        FlowersModelGenerationSystem,
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
        WoodModelGenerationSystem,
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
