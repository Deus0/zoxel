void prefab_set_mesh_indicies(ecs *world, entity e, const int* indicies_, int length) {
    if (!indicies_ || !zox_has(e, MeshIndicies)) {
        return;
    }
    MeshIndicies indicies = (MeshIndicies) { 0 };
    resize_MeshIndicies(&indicies, length);
    memcpy(indicies.value, indicies_, length * sizeof(int));
    zox_prefab_set_ptr(e, MeshIndicies, indicies);
}

void prefab_set_mesh2D_vertices(ecs *world, entity e, const float2* vertices_, int length) {
    if (!vertices_ || !zox_has(e, MeshVertices2D)) {
        return;
    }
    MeshVertices2D vertices = (MeshVertices2D) { 0 };
    resize_MeshVertices2D(&vertices, length);
    memcpy(vertices.value, vertices_, length * sizeof(float2));
    zox_prefab_set_ptr(e, MeshVertices2D, vertices);
}

void prefab_set_mesh3D_vertices(ecs *world, entity e, const float2* vertices, int length, float2 scale) {

    zox_mut_begin(e, MeshVertices, verts);
    if (verts->value) {
        dispose_MeshVertices(verts);
        zox_mut_end(e, MeshVertices);
    }
    MeshVertices meshVertices = (MeshVertices) { 0 };
    resize_MeshVertices(&meshVertices, length);
    for (int i = 0; i < length; i++) {
        meshVertices.value[i] = (float3) {
            vertices[i].x * scale.x,
            vertices[i].y * scale.y,
            0
        };
    }
    zox_prefab_set_ptr(e, MeshVertices, meshVertices);
}

void prefab_set_mesh_colors_rgb(ecs *world, entity e, color_rgb color, int length) {
    MeshColorRGBs *meshColorRGBs = &((MeshColorRGBs) { 0 });
    resize_MeshColorRGBs(meshColorRGBs, length);
    for (int i = 0; i < length; i++) meshColorRGBs->value[i] = color;
    zox_prefab_set(e, MeshColorRGBs, { meshColorRGBs->length, meshColorRGBs->value });
}

void prefab_set_mesh_vertices_float(ecs *world, entity e, const float* vertices, int length) {
    MeshVertices *meshVertices = &((MeshVertices) { 0 });
    resize_MeshVertices(meshVertices, length);
    memcpy(meshVertices->value, vertices, length * sizeof(float));
    zox_prefab_set(e, MeshVertices, { meshVertices->length, meshVertices->value })
}

void prefab_set_mesh_vertices_float3(ecs *world, entity e, const float3* vertices, int length) {
    MeshVertices *meshVertices = &((MeshVertices) { 0 });
    initialize_MeshVertices(meshVertices, length);
    if (length > 0) memcpy(meshVertices->value, vertices, length * sizeof(float3));
    zox_prefab_set(e, MeshVertices, { meshVertices->length, meshVertices->value });
}

void set_mesh_vertices_scale2D(MeshVertices2D* verts, const float2* new_vertices, int length, float2 scale2D) {
    resize_MeshVertices2D(verts, length);
    if (length > 0) {
        memcpy(verts->value, new_vertices, length * sizeof(float2));
    }
    for (int i = 0; i < length; i++) {
        float2_multiply_float2_p(&verts->value[i], scale2D);
    }
}

void prefab_set_mesh_uvs_float(ecs *world, entity e, const float* uvs, int length) {
    MeshUVs *meshUVs = &((MeshUVs) { 0, NULL });
    resize_MeshUVs(meshUVs, length);
    memcpy(meshUVs->value, uvs, length * sizeof(float));
    zox_prefab_set(e, MeshUVs, { meshUVs->length, meshUVs->value });
}

void prefab_set_mesh_uvs_float2(ecs *world, entity e, const float2* uvs, int length) {
    MeshUVs *mesh_uvs = &((MeshUVs) { 0 });
    resize_MeshUVs(mesh_uvs, length);
    if (mesh_uvs->value) {
        memcpy(mesh_uvs->value, uvs, length * sizeof(float2));
    } else {
        zox_loge("mesh_uvs realloc failed")
    }
    zox_prefab_set(e, MeshUVs, { mesh_uvs->length, mesh_uvs->value });
}
