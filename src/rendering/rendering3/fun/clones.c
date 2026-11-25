// a gpu mesh's (GpuMesh: meshed by a compute shader, chunks3/sys/vox_gpu.c) data read back into cpu arrays, for a clone
//  (an item, a mug shot, a mirror: drawn as a plain colored mesh); it waits for the gpu, clones are rare
//  - its vertices vec4s, its colours rgba8, its indices its count's
static inline byte mesh3_read_gpu(ecs* world, entity mesh, MeshIndicies* indicies, MeshVertices* verts, MeshColorRGBs* colors) {
    const int count = mesh_index_count(world, mesh);
    const guint2 buffers = zox_getv(mesh, MeshGPULink);
    const guint colors_buffer = zox_getv(mesh, ColorsGPULink);
    if (!count || !buffers.x || !buffers.y || !colors_buffer) {
        return 0;
    }
    const int vertices = count / 6 * 4;
    float4* vertex4s = malloc(sizeof(float4) * vertices);
    uint* rgbas = malloc(sizeof(uint) * vertices);
    initialize_MeshIndicies(indicies, count);
    initialize_MeshVertices(verts, vertices);
    initialize_MeshColorRGBs(colors, vertices);
    zox_gpu_finish();
    zox_gpu_storage_read(buffers.x, 0, sizeof(int) * count, indicies->value);
    zox_gpu_storage_read(buffers.y, 0, sizeof(float4) * vertices, vertex4s);
    zox_gpu_storage_read(colors_buffer, 0, sizeof(uint) * vertices, rgbas);
    for (int i = 0; i < vertices; i++) {
        verts->value[i] = (float3) { vertex4s[i].x, vertex4s[i].y, vertex4s[i].z };
        colors->value[i] = (color_rgb) { rgbas[i] & 255, (rgbas[i] >> 8) & 255, (rgbas[i] >> 16) & 255 };
    }
    free(vertex4s);
    free(rgbas);
    return 1;
}

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
    if (zox_has(clonee, GpuMesh)) {
        MeshIndicies indicies = { 0 };
        MeshVertices verts = { 0 };
        MeshColorRGBs colors = { 0 };
        if (!mesh3_read_gpu(world, clonee, &indicies, &verts, &colors)) {
            zox_loge("Mesh3 gpu source not meshed [%s]", zox_getn(clonee));
            return 0;
        }
        entity e = zox_ins(world, prefab_mesh3);
        zox_setv(e, MeshRenderCount, indicies.length);
        zox_set_ptr(e, MeshIndicies, indicies);
        zox_set_ptr(e, MeshVertices, verts);
        zox_set_ptr(e, MeshColorRGBs, colors);
        zox_add(e, MeshDirty);
        zox_link(world, e, MeshCloneLink, clonee);
        zox_add(clonee, MeshCloned);
        return e;
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

// source's data into its clones (a gpu mesh's read back once)
static inline void mesh_clones_fill(ecs* world, entity source, const entity* clones, int count) {
    MeshIndicies gpu_indicies = { 0 };
    MeshVertices gpu_verts = { 0 };
    MeshColorRGBs gpu_colors = { 0 };
    const byte gpu = zox_has(source, GpuMesh) && mesh3_read_gpu(world, source, &gpu_indicies, &gpu_verts, &gpu_colors);
    const MeshIndicies* indicies = gpu ? &gpu_indicies : zox_get(source, MeshIndicies);
    const MeshVertices* verts = gpu ? &gpu_verts : zox_get(source, MeshVertices);
    const MeshColorRGBs* colors = gpu ? &gpu_colors : zox_get(source, MeshColorRGBs);
    for (int j = 0; j < count; j++) {
        const entity clone = clones[j];
        if (!zox_valid(clone) || !zox_has(clone, MeshIndicies) || !indicies->value || !verts->value || !colors->value) {
            continue;
        }
        clone_MeshIndicies(zox_get_mut(clone, MeshIndicies), indicies);
        clone_MeshVertices(zox_get_mut(clone, MeshVertices), verts);
        clone_MeshColorRGBs(zox_get_mut(clone, MeshColorRGBs), colors);
        zox_setv(clone, MeshRenderCount, indicies->length);
        zox_add(clone, MeshDirty);
    }
    if (gpu) {
        dispose_MeshIndicies(&gpu_indicies);
        dispose_MeshVertices(&gpu_verts);
        dispose_MeshColorRGBs(&gpu_colors);
    }
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
    for (int j = 0; j < count; j++) {
        if (zox_valid(clones[j])) {
            zox_unlink(world, clones[j], MeshCloneLink, from);
            zox_link(world, clones[j], MeshCloneLink, to);
        }
    }
    mesh_clones_fill(world, to, clones, count);
    zox_add(to, MeshCloned);
}

// a gpu mesh meshed again: its clones copy it (a cpu mesh's do on MeshDirty, mesh_clone_sync_event)
void mesh_clones_sync_gpu(ecs* world, entity source) {
    if (!zox_has(source, MeshCloned)) {
        return;
    }
    entity clones[64];
    const int count = mesh_clones_of(world, source, clones, 64);
    mesh_clones_fill(world, source, clones, count);
}

// a colored mesh's faces as voxel faces of a size: its quads' area over a voxel face's, the same however its faces are
//  merged (greedy meshing, chunks3/fun/greedy.c); a gpu mesh read back (tests)
int mesh3_voxel_faces(ecs* world, entity mesh, float voxel) {
    MeshIndicies gpu_indicies = { 0 };
    MeshVertices gpu_vertices = { 0 };
    MeshColorRGBs gpu_colors = { 0 };
    const byte gpu = zox_has(mesh, GpuMesh) && mesh3_read_gpu(world, mesh, &gpu_indicies, &gpu_vertices, &gpu_colors);
    const MeshVertices* vertices = gpu ? &gpu_vertices : zox_get(mesh, MeshVertices);
    double area = 0;
    for (int i = 0; vertices && i + 3 < vertices->length; i += 4) {
        const float3* v = &vertices->value[i];
        area += float3_length(float3_cross(float3_subtract(v[1], v[0]), float3_subtract(v[3], v[0])));
    }
    if (gpu) {
        dispose_MeshIndicies(&gpu_indicies);
        dispose_MeshVertices(&gpu_vertices);
        dispose_MeshColorRGBs(&gpu_colors);
    }
    return (int) lround(area / ((double) voxel * voxel));
}
