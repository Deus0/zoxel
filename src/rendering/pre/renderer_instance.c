entity spawn_prefab_renderer_instance(ecs *world) {
    zox_prefab();
    zox_prefab_name("renderer_instance");
    zox_add(e, RendererInstance);
    zox_setv(e, Brightness, 1);
    zox_setv(e, RenderDisabled, 0);
    zox_setv(e, TransformMatrix, float4x4_identity);
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, float4_identity);
    zox_setv(e, Scale1, 1);
#ifdef zox_non_fragment_parent
    // zox_add(e, EcsParent);
#endif
    return e;
}

entity spawn_prefab_static_instance_mesh(ecs *world) {
    zox_prefab();
    zox_prefab_name("renderer_instance");
    zox_add(e, RendererInstance);
    zox_setv(e, Brightness, 1);
    zox_setv(e, RenderDisabled, 0);
    zox_setv(e, TransformMatrix, float4x4_identity);
#ifdef zox_non_fragment_parent
    // zox_add(e, EcsParent);
#endif
    return e;
}
