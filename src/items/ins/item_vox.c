entity spawn_item_vox_mesh(ecs* world, entity item) {
    entity model = zox_has(item, ModelLink) ?
        zox_getv(item, ModelLink) :
        0;
    entity mesh = get_max_model_mesh(world, model);
    if (!mesh) {
        zox_loge("[spawn_item_vox_mesh] Invalid Model [mesh]:[%s]",
            zox_getn(model));
        return 0;
    }
    if (!zox_has(mesh, Mesh)) {
        zox_loge("[spawn_item_vox_mesh] Invalid [mesh] [%s]: No Mesh on Entity [%s]",
            zox_getn(model),
            zox_getn(mesh));
        return 0;
    }
    // float3 camera_position = zox_getv(camera, Position3D);
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
