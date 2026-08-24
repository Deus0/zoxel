entity spawn_xr_eye(ecs* world, entity parent) {
    entity e = zox_new();
    zox_set_parent(world, e, parent);
    // zox_add(e, Camera3);
    zox_add(e, XrCamera);
    zox_add(e, XrFov);          // get thesefrom xr?
    /*
    zox_add(e, Camera);
    zox_add(e, TransformMatrix);
    zox_add(e, ProjectionMatrix);
    zox_add(e, ViewProjectionMatrix);
    zox_setv(e, CameraNearDistance, 0.01f);
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, LocalPosition3D, float3_zero);
    zox_setv(e, Rotation3D, float4_identity);
    zox_setv(e, LocalRotation3D, quaternion_identity);
    zox_setv(e, Initialize, 1);
    add_prefab_render_camera(world, e);
    // For our rendering
    zox_set(e, FieldOfView, 90);*/
    return e;
}
