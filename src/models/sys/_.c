#include "realm.c"
#include "generation.c"
zox_declare_system_state_event(RealmModels, GenerateRealm, zox_generate_realm_models, spawn_realm_models)

void define_systems_models(ecs* world) {
    zox_define_system_state_event_1(RealmModels, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm);

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