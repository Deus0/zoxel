void fetch_neightbor_chunk_data(ecs* world, const ChunkNeighbors* chunk_neighbors, const VoxelNode** neighbors, byte* ndepths) {
    for (int i = 0; i < 6; i++) {
        entity e = chunk_neighbors->value[i];

        if (!zox_valid(e) || !zox_has(e, RenderDepth) || !zox_has(e, VoxelNode)) {
            ndepths[i] = 0;
            neighbors[i] = 0;
            continue;
        }

        neighbors[i] = zox_get(e, VoxelNode);
        zox_geter_value(e, RenderDepth, byte, render_depth);
        ndepths[i] = render_depth;
    }
}

// this takes 14ms on a 24core cpu, 6ms though during streaming
// scales vertex, offsets vertex by voxel position in chunk, adds total mesh offset
void zox_build_voxel_face(const mesh_uvs_build_data* mesh, const int* indicies, const float3* verts, const float2* uvs, float3 offset, float scale) {

    // indicies
    expand_capacity_int_array_d(mesh->indicies, voxel_face_indicies_length);
    for (byte i = 0; i < 6; i++) {
        mesh->indicies->data[mesh->indicies->size + i] = mesh->vertices->size + indicies[i];
    }
    mesh->indicies->size += voxel_face_indicies_length;

    // verts
    expand_capacity_float3_array_d(mesh->vertices, voxel_face_vertices_length);
    for (byte i = 0; i < voxel_face_vertices_length; i++) {
        float3 vertex_position = verts[i];
        float3_add_float3_p(&vertex_position, offset);
        float3_scale_p(&vertex_position, scale);
        mesh->vertices->data[mesh->vertices->size + i] = vertex_position;
    }
    mesh->vertices->size += voxel_face_vertices_length;

    // uvs
    expand_capacity_float2_array_d(mesh->uvs, voxel_face_vertices_length);
    for (byte i = 0; i < 4; i++) {
        const float2 vert_uv = uvs[i];
        mesh->uvs->data[mesh->uvs->size + i] = vert_uv;
    }
    mesh->uvs->size += voxel_face_vertices_length;

    // colors
    for (byte a = 0; a < voxel_face_vertices_length; a++) {
        add_to_color_rgb_array_d(mesh->color_rgbs, color_rgb_white);
    }
}