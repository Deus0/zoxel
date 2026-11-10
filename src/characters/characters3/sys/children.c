// Characters hold no voxel or mesh data, their render children are spawned here
//  - with a model: a renderer child instancing its models lod mesh
//  - CharacterUnique: a cloned vox child with its own lod meshes, and a renderer child for those
//  - SkeletonLink: a combined vox child for its body parts, and a skeleton child
// NOTE: Spawned from a system so meta characters (prefabs) don't get children
zox_sys2(CharacterChildrenSpawnSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateCharacter);
    zox_sys_in(RenderDepth);
    zox_sys_in(ModelLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateCharacter, state);
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_i(ModelLink, model);
        if (state->value != zox_dirty_active) {
            continue;
        }
        if (zox_has(e, SkeletonLink)) {
            spawn_vox_combined_child(world, e);
            spawn_skeleton(world, e);
            continue;
        }
        if (!zox_valid(model->value)) {
            continue;
        }
        entity mesh = 0;
        if (zox_has(e, CharacterUnique)) {
            // NOTE: VoxLink is deferred, so we use the lods returned here
            ModelLods lods = { 0 };
            if (!spawn_vox_cloned_child(world, e, model->value, &lods)) {
                continue;
            }
            if (rdepth->value < model_lods_max_length) {
                mesh = lods.value[rdepth->value];
            }
        } else {
            mesh = get_owner_lod_mesh(world, e, rdepth->value);
        }
        spawn_renderer_child(world, prefab_renderer_child, e, mesh);
        if (dbg_log) {
            zox_log("[%s] spawned its render children", zox_sys_e_name);
        }
    }
} zox_sys_end(CharacterChildrenSpawnSystem);
