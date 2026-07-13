#include "generation.c"
#include "colors.c"
#include "fill.c"
#include "paint.c"
#include "body.c"
#include "merge.c"
#include "blocks.c"
#include "settings.c"
realm_clear_system(ModelLinks);

void define_systems_models(ecs* world) {
    realm_clear_systemd(rendering, ModelLinks);
    // NOTE: Writes to VoxelNode
    zox_system(
        VoxGenerationSystem,
        zoxp_voxels_write,
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
        zoxp_voxels_write,
        [in] nodes.NodeBegin,
        [in] nodes.NodeLink,
        [in] rendering.ModelLink,
        [out] nodes.NodeEnd
    );
    zox_system(
        ColorsModelNodeSystem,
        zoxp_voxels_write,
        [in] nodes.NodeBegin,
        [in] nodes.NodeLink,
        [in] rendering.ModelLink,
        [out] nodes.NodeEnd
    );
    zox_system(
        PaintModelNodeSystem,
        zoxp_voxels_write,
        [in] nodes.NodeBegin,
        [in] nodes.NodeLink,
        [in] rendering.ModelLink,
        [out] nodes.NodeEnd
    );
    zox_system_1(
        ModelsSettingsSystem,
        zoxp_mainthread,
        [in] core.InitializeEntity,
        [none] apps.App
    );
    zox_system_1(
        ModelsSettingsDirtySystem,
        zoxp_mainthread,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
