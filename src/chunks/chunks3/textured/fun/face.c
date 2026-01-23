

// this takes 14ms on a 24core cpu, 6ms though during streaming
// scales vertex, offsets vertex by voxel position in chunk, adds total mesh offset
void zox_build_voxel_face(const mesh_uvs_build_data* mesh, const int* indicies, const float3* verts, const float2* uvs, float3 position, float3 scale3) {

    // indicies
    expand_capacity_int_array_d(mesh->indicies, voxel_face_indicies_length);

    for (byte i = 0; i < 6; i++) {

        int j = mesh->indicies->size + i;

        mesh->indicies->data[j] = mesh->vertices->size + indicies[i];

    }

    mesh->indicies->size += voxel_face_indicies_length;

    // verts
    expand_capacity_float3_array_d(mesh->vertices, voxel_face_vertices_length);

    for (byte i = 0; i < voxel_face_vertices_length; i++) {

        int j = mesh->vertices->size + i;

        float3 vert = verts[i];

        float3_scale3p(&vert, scale3);

        float3_add_float3_p(&vert, position);
        // float3_scale_p(&vert, scale);

        mesh->vertices->data[j] = vert;

    }

    mesh->vertices->size += voxel_face_vertices_length;

    // uvs
    expand_capacity_float2_array_d(mesh->uvs, voxel_face_vertices_length);

    for (byte i = 0; i < 4; i++) {

        int j = mesh->uvs->size + i;

        mesh->uvs->data[j] = uvs[i];

    }

    mesh->uvs->size += voxel_face_vertices_length;

    // White for now, alter in other systems
    for (byte a = 0; a < voxel_face_vertices_length; a++) {

        add_to_color_rgb_array_d(mesh->color_rgbs, color_rgb_white);

    }
}