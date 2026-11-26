// a lit mesh from its arrays (copied, then freed), at a position, a child of parent if it has one
entity spawn_lit_mesh(ecs* world, entity parent, float3 position, MeshVertices vertices, MeshIndicies indices, MeshColors colors) {
    const entity e = zox_ins(world, prefab_lit_mesh);
    zox_setv(e, Position3D, position);
    zox_set_ptr(e, MeshVertices, vertices);
    zox_set_ptr(e, MeshIndicies, indices);
    zox_set_ptr(e, MeshColors, colors);
    dispose_MeshVertices(&vertices);
    dispose_MeshIndicies(&indices);
    dispose_MeshColors(&colors);
    zox_link(world, e, MaterialLink, material_colored3D_lit);
    zox_add(e, MeshDirty);
    if (parent) {
        zox_set_parent(world, e, parent);
    }
    return e;
}
