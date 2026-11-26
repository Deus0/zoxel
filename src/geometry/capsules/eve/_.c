// a capsule mesh changed (Dirty): its mesh built from its shape, then uploaded (MeshDirty)
static void capsule_mesh_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RadiusFloat);
    zox_sys_in(HeightFloat);
    zox_sys_in(ColorRGB);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RadiusFloat, radius);
        zox_sys_i(HeightFloat, height);
        zox_sys_i(ColorRGB, tint);
        build_capsule_mesh(zox_get_mut(e, MeshVertices), zox_get_mut(e, MeshIndicies), zox_get_mut(e, MeshColors),
            radius->value, height->value, capsule_mesh_segments, tint->value);
        zox_add(e, MeshDirty);
    }
}

static void zox_events_capsules(ecs* world) {
    zox_on_add(capsule_mesh_event, [in] core.RadiusFloat, [in] core.HeightFloat, [in] colorz.ColorRGB,
        [none] capsules.CapsuleMesh, [none] core.Dirty);
}
