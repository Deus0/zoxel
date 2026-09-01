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
        NoiseVoxelNodeSystem,
        zoxp_update,
        [in] chunks.NodeDepth,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] colorz.ColorRGBs,
        [none] chunks3.NoiseChunk
    );
    zox_system(
        DecayedModelGenerationSystem,
        zoxp_update,
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
        zoxp_update,
        [in] core.Seed,
        [in] colorz.Color,
        [in] models.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
    zox_system(
        SoilGenerationSystem,
        zoxp_update,
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
        zoxp_update,
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
        zoxp_update,
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
        zoxp_update,
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
        zoxp_update,
        [in] core.Seed,
        [in] colorz.Color,
        [in] models.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
    zox_system(
        RubbleModelGenerationSystem,
        zoxp_update,
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
        zoxp_update,
        [in] core.Seed,
        [in] colorz.Color,
        [in] models.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
}
