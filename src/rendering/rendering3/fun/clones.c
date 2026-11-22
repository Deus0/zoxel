entity spawn_mesh3_clone(
    ecs* world,
    entity clonee)
{
    if (!zox_valid(clonee) ||
        !zox_has(clonee, Mesh)) {
        zox_loge("[spawn_mesh3_clone] Invalid clonee [%s]",
            zox_getn(clonee));
        return 0;
    }
    // get mesh data
    const MeshIndicies* source_indicies = zox_get(clonee, MeshIndicies);
    const MeshVertices* source_verts = zox_get(clonee, MeshVertices);
    const MeshColorRGBs* source_colors = zox_get(clonee, MeshColorRGBs);
    if (!source_indicies->value ||
        !source_verts->value ||
        !source_colors->value)
    {
        zox_loge("Mesh3 Invalid Source [%s]",
            zox_getn(clonee));
        return 0;
    }
    // Initialize new data
    MeshIndicies indicies = { 0 };
    MeshVertices verts = { 0 };
    MeshColorRGBs colors = { 0 };
    initialize_MeshIndicies(
        &indicies,
        source_indicies->length);
    initialize_MeshVertices(
        &verts,
        source_verts->length);
    initialize_MeshColorRGBs(
        &colors,
        source_colors->length);
    if (!indicies.value ||
        !verts.value ||
        !colors.value)
    {
        zox_loge("Mesh3 data malloc failed clonee [%s]",
            zox_getn(clonee));
        return 0;
    }
    memcpy(
        indicies.value,
        source_indicies->value,
        source_indicies->length * sizeof(int));
    memcpy(
        verts.value,
        source_verts->value,
        source_verts->length * sizeof(float3));
    memcpy(
        colors.value,
        source_colors->value,
        source_colors->length * sizeof(color_rgb));
    entity e = zox_ins(world, prefab_mesh3);
    zox_set_ptr(e, MeshIndicies, indicies);
    zox_set_ptr(e, MeshVertices, verts);
    zox_set_ptr(e, MeshColorRGBs, colors);
    zox_setv(e, MeshRenderCount, zox_getv(clonee, MeshRenderCount));
    zox_add(e, MeshDirty);
    // keep clone in sync with source, see mesh_clone_sync_event
    zox_link(world, e, MeshCloneLink, clonee);
    zox_add(clonee, MeshCloned);
    return e;
}

// a source's clones, those linked to it (MeshCloneLink), up to max
static inline int mesh_clones_of(ecs* world, entity source, entity* clones, int max) {
    int count = 0;
    iter it = ecs_each_id(world, ecs_pair(zox_id(MeshCloneLink), source));
    while (zox_each_next(it)) {
        for (int j = 0; j < it.count; j++) {
            if (count < max && it.entities[j] != source) {
                clones[count++] = it.entities[j];
            }
        }
    }
    return count;
}

// a mesh swapped for another (a skeleton's rebuilt mesh, skeleton_meshes.c): its clones follow the new one, given its
//  data now (a mirror, a mug shot kept the old mesh: the old hat, tst mirror_hat)
void mesh_clones_move(ecs* world, entity from, entity to) {
    if (!zox_valid(from) || !zox_valid(to) || from == to || !zox_has(from, MeshCloned)) {
        return;
    }
    entity clones[64];
    const int count = mesh_clones_of(world, from, clones, 64);
    if (!count) {
        return;
    }
    const MeshIndicies* indicies = zox_get(to, MeshIndicies);
    const MeshVertices* verts = zox_get(to, MeshVertices);
    const MeshColorRGBs* colors = zox_get(to, MeshColorRGBs);
    for (int j = 0; j < count; j++) {
        const entity clone = clones[j];
        if (!zox_valid(clone) || !zox_has(clone, MeshIndicies)) {
            continue;
        }
        zox_unlink(world, clone, MeshCloneLink, from);
        zox_link(world, clone, MeshCloneLink, to);
        if (indicies->value && verts->value && colors->value) {
            clone_MeshIndicies(zox_get_mut(clone, MeshIndicies), indicies);
            clone_MeshVertices(zox_get_mut(clone, MeshVertices), verts);
            clone_MeshColorRGBs(zox_get_mut(clone, MeshColorRGBs), colors);
            zox_setv(clone, MeshRenderCount, zox_getv(to, MeshRenderCount));
            zox_add(clone, MeshDirty);
        }
    }
    zox_add(to, MeshCloned);
}
