#include "generation.c"
#include "soil.c"
#include "colors.c"
#include "fill.c"
#include "paint.c"
#include "body.c"
#include "merge.c"
#include "blocks.c"
#include "settings.c"
#include "road.c"
#include "sand.c"
realm_clear_system(ModelLinks);

void define_systems_models(ecs* world) {
    realm_clear_systemd(rendering, ModelLinks);
    // NOTE: Writes to VoxelNode
    zox_system(
        VoxGenerationSystem,
        zoxp_update,
        [in] core.Seed,
        [in] colorz.Color,
        [in] voxes.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeDirty,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
    zox_system(
        SoilGenerationSystem,
        zoxp_update,
        [in] colorz.Color,
        [in] voxes.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeDirty,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
    zox_system(
        RoadModelGenerationSystem,
        zoxp_update,
        [in] colorz.Color,
        [in] voxes.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeDirty,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
    zox_system(
        SandModelGenerationSystem,
        zoxp_update,
        [in] colorz.Color,
        [in] voxes.VoxType,
        [out] chunks.GenerateModel,
        [out] chunks3.VoxelNode,
        [out] chunks3.VoxelNodeDirty,
        [out] chunks.NodeDepth,
        [out] colorz.ColorRGBs
    );
    zox_system(
        FillModelNodeSystem,
        zoxp_update,
        [in] nodes.NodeBegin,
        [in] nodes.NodeLink,
        [in] rendering.ModelLink,
        [out] nodes.NodeEnd
    );
    zox_system(
        ColorsModelNodeSystem,
        zoxp_update,
        [in] nodes.NodeBegin,
        [in] nodes.NodeLink,
        [in] rendering.ModelLink,
        [out] nodes.NodeEnd
    );
    zox_system(
        PaintModelNodeSystem,
        zoxp_update,
        [in] nodes.NodeBegin,
        [in] nodes.NodeLink,
        [in] rendering.ModelLink,
        [out] nodes.NodeEnd
    );
    zox_system_1(
        ModelsSettingsSystem,
        zoxp_mainthread,
        [in] settings.LoadSettings,
    );
    zox_system_1(
        ModelsSettingsDirtySystem,
        zoxp_mainthread,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
