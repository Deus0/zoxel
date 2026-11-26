// a capsule mesh, a child of parent (a character) if it has one, its centre on it: built by its observer (Dirty)
entity spawn_capsule_mesh(ecs* world, entity parent, float radius, float height, color_rgb tint) {
    const entity e = zox_ins(world, prefab_capsule_mesh);
    zox_setv(e, RadiusFloat, radius);
    zox_setv(e, HeightFloat, height);
    zox_setv(e, ColorRGB, tint);
    zox_link(world, e, MaterialLink, material_colored3D_lit);
    if (parent) {
        zox_set_parent(world, e, parent);
    }
    zox_add(e, Dirty);
    return e;
}
