#include "realm.c"
#include "generation.c"
realm_clear_system(ModelLinks);

void define_systems_models(ecs* world) {

    realm_clear_systemd(rendering, ModelLinks);

    zox_system_1(
        ModelsRealmSpawnSystem,
        EcsOnLoad,
        [in] realms.GenerateRealm,
        [in] colorz.Colors,
        [out] rendering.ModelLinks,
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
}