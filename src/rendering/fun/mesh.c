void prefab_set_mesh_indicies(
    ecs *world,
    entity e,
    const int* indicies_,
    int length
) {
    if (headless || !indicies_ || !zox_has(e, MeshIndicies)) {
        return;
    }
    MeshIndicies indicies = (MeshIndicies) { 0 };
    initialize_MeshIndicies(&indicies, length);
    memcpy(indicies.value, indicies_, length * sizeof(int));
    zox_prefab_set_ptr(e, MeshIndicies, indicies);
}

void prefab_set_mesh2D_vertices(
    ecs *world,
    entity e,
    const float2* vertices_,
    int length
) {
    if (headless || !vertices_ || !zox_has(e, MeshVertices2D)) {
        return;
    }
    MeshVertices2D vertices = (MeshVertices2D) { 0 };
    initialize_MeshVertices2D(&vertices, length);
    memcpy(vertices.value, vertices_, length * sizeof(float2));
    zox_prefab_set_ptr(e, MeshVertices2D, vertices);
}

void prefab_set_mesh3D_vertices(
    ecs *world,
    const entity e,
    const float2* vertices,
    const int length,
    const float2 scale
) {
    if (headless) {
        return;
    }
    zox_mut_begin(e, MeshVertices, verts);
    if (verts->value) {
        dispose_MeshVertices(verts);
        zox_mut_end(e, MeshVertices);
    }
    MeshVertices meshVertices = (MeshVertices) { 0 };
    initialize_MeshVertices(&meshVertices, length);
    for (int i = 0; i < length; i++) {
        meshVertices.value[i] = (float3) {
            vertices[i].x * scale.x,
            vertices[i].y * scale.y,
            0
        };
    }
    zox_prefab_set_ptr(e, MeshVertices, meshVertices);
}

void prefab_set_mesh_colors_rgb(
    ecs *world,
    entity e,
    color_rgb color,
    int length
) {
    if (headless) {
        return;
    }
    MeshColorRGBs *meshColorRGBs = &((MeshColorRGBs) { 0, NULL });
    initialize_MeshColorRGBs(meshColorRGBs, length);
    for (int i = 0; i < length; i++) meshColorRGBs->value[i] = color;
    zox_prefab_set(e, MeshColorRGBs, { meshColorRGBs->length, meshColorRGBs->value })
}

void prefab_set_mesh_vertices_float(
    ecs *world,
    entity e,
    const float* vertices,
    int length
) {
    if (headless) {
        return;
    }
    MeshVertices *meshVertices = &((MeshVertices) { 0, NULL });
    initialize_MeshVertices(meshVertices, length);
    memcpy(meshVertices->value, vertices, length * sizeof(float));
    zox_prefab_set(e, MeshVertices, { meshVertices->length, meshVertices->value })
}

void prefab_set_mesh_vertices_float3(
    ecs *world,
    entity e,
    const float3* vertices,
    int length
) {
    if (headless) {
        return;
    }
    MeshVertices *meshVertices = &((MeshVertices) { 0, NULL });
    initialize_MeshVertices(meshVertices, length);
    if (length > 0) memcpy(meshVertices->value, vertices, length * sizeof(float3));
    zox_prefab_set(e, MeshVertices, { meshVertices->length, meshVertices->value })
}

void set_mesh_vertices_scale2D(
    MeshVertices2D *meshVertices2D,
    const float2* new_vertices,
    const int length,
    const float2 scale2D
) {
    initialize_MeshVertices2D(meshVertices2D, length);
    if (length > 0) {
        memcpy(meshVertices2D->value, new_vertices, length * sizeof(float2));
    }
    for (int i = 0; i < length; i++) {
        float2_multiply_float2_p(&meshVertices2D->value[i], scale2D);
    }
}

void prefab_set_mesh_uvs_float(
    ecs *world,
    entity e,
    const float* uvs,
    int length
) {
    if (headless) {
        return;
    }
    MeshUVs *meshUVs = &((MeshUVs) { 0, NULL });
    initialize_MeshUVs(meshUVs, length);
    memcpy(meshUVs->value, uvs, length * sizeof(float));
    zox_prefab_set(e, MeshUVs, { meshUVs->length, meshUVs->value })
}

void prefab_set_mesh_uvs_float2(
    ecs *world,
    entity e,
    const float2* uvs,
    int length
) {
    if (headless) {
        return;
    }
    MeshUVs *meshUVs = &((MeshUVs) { 0, NULL });
    initialize_MeshUVs(meshUVs, length);
    if (meshUVs->value) {
        memcpy(meshUVs->value, uvs, length * sizeof(float2));
    } else {
        zox_log_error("meshUVs realloc failed")
    }
    zox_prefab_set(e, MeshUVs, { meshUVs->length, meshUVs->value })
}
