entity spawn_mesh3_clone(
    ecs* world,
    entity clonee)
{
    // get mesh data
    const MeshIndicies* source_indicies = zox_get(clonee, MeshIndicies);
    const MeshVertices* source_verts = zox_get(clonee, MeshVertices);
    const MeshColorRGBs* source_colors = zox_get(clonee, MeshColorRGBs);
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
        zox_loge("Mesh3 data malloc failed");
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
    return e;
}