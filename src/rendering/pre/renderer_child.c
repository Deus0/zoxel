// An instance renderer that follows its parent, i.e. a characters model
//  - transform comes from the transform hierarchy, RenderDisabled is set with its parent
//  - InstanceLink is the lod mesh to render, set by RendererLodSystem
entity spawn_prefab_renderer_child(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("renderer_child");
    zox_setv(e, InstanceLink, 0);
    zox_setv(e, LocalPosition3D, float3_zero);
    zox_setv(e, LocalRotation3D, quaternion_identity);
    return e;
}

// Spawns a renderer child, linked from its parent with RendererLink
entity spawn_renderer_child(
    ecs* world,
    entity prefab,
    entity parent,
    entity mesh)
{
    entity e = zox_ins(world, prefab);
    zox_set_unique_name(e, "renderer_child");
    zox_setv(e, InstanceLink, mesh);
    if (zox_has(parent, RenderDisabled)) {
        zox_setv(e, RenderDisabled, zox_getv(parent, RenderDisabled));
    }
    if (zox_has(parent, Position3D)) {
        zox_setv(e, Position3D, zox_getv(parent, Position3D));
    }
    if (zox_has(parent, Rotation3D)) {
        zox_setv(e, Rotation3D, zox_getv(parent, Rotation3D));
    }
    zox_set_parent(world, e, parent);
    zox_set_link(world, parent, RendererLink, e);
    return e;
}
