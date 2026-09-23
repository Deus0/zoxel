// extern entity spawn_inspector(ecs*, entity, entity, entity);

entity spawn_entity_mesh_clone(
    ecs* world,
    entity e,
    float3 spawn_position,
    float rotate_speed)
{
    if (!e) {
        zox_loge("Invalid [e]");
        return 0;
    }
    entity model = zox_has(e, ModelLink) ?
        zox_getv(e, ModelLink) :
        0;
    if (!model) {
        zox_loge("Invalid [model]");
        return 0;
    }
    entity mesh = get_max_model_mesh(world, model);
    if (!mesh) {
        zox_loge("[dbg_block_vox_mesh] Invalid Model [mesh]:[%s]", zox_getn(model));
        return 0;
    }
    if (!zox_has(mesh, Mesh)) {
        zox_loge("[dbg_block_vox_mesh] Invalid [mesh] [%s]: No Mesh", zox_getn(mesh));
        return 0;
    }
    // float3 camera_position = zox_getv(camera, Position3D);
    entity clone = spawn_mesh3_clone(world, mesh);
    if (!clone) {
        zox_loge("Invalid [clone] on [%s]:[%s]",
            zox_getn(e),
            zox_getn(model));
        return 0;
    }
    zox_add(clone, VoxMesh);
    zox_set_unique_name(clone, "block_item_mesh");
    zox_setv(clone, Position3D, spawn_position);
    zox_setv(clone, Rotation3D, quaternion_identity);
    add_eternal_euler(
        world,
        clone,
        (float3) { 0, rotate_speed, 0 });
    return clone;
}

#include "block.c"
#include "block_vox_mesh.c"
#include "item_mesh.c"
