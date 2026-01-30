// NOTE: Scales vertex, offsets vertex by voxel position in chunk, adds total mesh offset
void add_voxel_face(mesh_colored_build_data* mesh, float3 position, float3 bounds_offset, float scale, const int* indiciesf, const float3* verticesf) {

    expand_capacity_int_array_d(mesh->indicies, voxel_face_indicies_length);

    for (int i = 0, j = mesh->indicies->size; i < voxel_face_indicies_length; i++, j++) {

        mesh->indicies->data[j] = mesh->vertices->size + indiciesf[i];

    }
    mesh->indicies->size += voxel_face_indicies_length;

    expand_capacity_float3_array_d(mesh->vertices, voxel_face_vertices_length);
    for (int i = 0, j = mesh->vertices->size; i < voxel_face_vertices_length; i++, j++) {

        float3 vertex = verticesf[i];

        float3_add_float3_p(&vertex, position);
        float3_scale_p(&vertex, scale);
        float3_add_float3_p(&vertex, bounds_offset);

        mesh->vertices->data[j] = vertex;

    }
    mesh->vertices->size += voxel_face_vertices_length;
}