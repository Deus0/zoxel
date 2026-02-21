#include "realm.c"
#include "generation.c"
#include "colors.c"
#include "fill.c"
#include "paint.c"
#include "body.c"
#include "merge.c"

realm_clear_system(ModelLinks);

void define_systems_models(ecs* world) {

    realm_clear_systemd(rendering, ModelLinks);

    zox_system_1(
        ModelsRealmSpawnSystem,
        EcsOnLoad,
        [in] realms.GenerateRealm,
        [in] colorz.Colors,
        [out] rendering.ModelLinks,
        [out] nodes.NodegraphLinks,
        [none] realms.Realm
    );

    // NOTE: Writes to VoxelNode
    zox_system(
        VoxGenerationSystem,
        zoxp_voxels_write,
        [in] core.Generate,
        [in] colorz.Color,
        [in] voxes.VoxType,
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
}
