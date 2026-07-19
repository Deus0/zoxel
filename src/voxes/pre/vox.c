entity spawn_prefab_vox(ecs *world, entity prefab, byte depth) {
    zox_prefab_child(prefab);
    zox_prefab_name("vox");
    zox_add_tag(e, Vox);
    // Rendering
    zox_prefab_set(e, MeshDirty, { 0 });
    zox_prefab_set(e, RenderDisabled, { 0 });
    // vox
    zox_set(e, BlockScale, { vox_model_scale });
    zox_set(e, NodeDepth, { depth }); // block_vox_depth });
    add_components_mesh_colored(world, e);
    // This is for Generation tho
    // zox_set(e, Generate, { 0 });
    // zox_prefab_set(e, VoxType, { vox_type_soil });
    // transform
    // zox_prefab_set(e, Euler, { float3_zero });
    // zox_prefab_set(e, Scale1D, { 1 });
    return e;
}
