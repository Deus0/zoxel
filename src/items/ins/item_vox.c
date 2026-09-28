entity spawn_item_vox_mesh(ecs* world, entity item) {
    entity model = zox_has(item, ModelLink) ?
        zox_getv(item, ModelLink) :
        0;
    entity mesh = get_max_model_mesh(world, model);
    if (!mesh) {
        zox_loge("[spawn_item_vox_mesh] Invalid [mesh]: item [%s], model [%s], mesh [%s]",
            zox_getn(item),
            zox_getn(model),
            zox_getn(mesh));
        return 0;
    }
    if (!zox_has(mesh, Mesh)) {
        zox_loge("[spawn_item_vox_mesh] Invalid Components [mesh]: item [%s], model [%s], mesh [%s]",
            zox_getn(item),
            zox_getn(model),
            zox_getn(mesh));
        return 0;
    }
    entity e = spawn_mesh3_clone(world, mesh);
    if (!e) {
        zox_loge("Invalid [mesh_clone]");
        return 0;
    }
    zox_add(e, VoxMesh);
    zox_set_unique_name(e, "item_mesh");
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, quaternion_identity);
    return e;
}
