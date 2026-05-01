// const byte isDebugRenderMaterial2DSystem = 0;

void set_gpu_mesh2D(
    uint2 mesh,
    const int *indicies,
    int indicies_length,
    const float2 *verts,
    int verts_length
) {
    gint vertex_shader_index = 0;
    int float_per_data = 2;
    int floats_length = verts_length * float_per_data;
    float combined_verts[floats_length];
    for (int i = 0; i < verts_length; i++) {
        float2 vert = verts[i];
        combined_verts[i * float_per_data + 0] = vert.x;
        combined_verts[i * float_per_data + 1] = vert.y;
    }
    zox_gpu_bind_buffer_element(mesh.x);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies_length * 4, indicies, GL_STATIC_DRAW);
    zox_gpu_set_buffer_element(indicies, indicies_length * 4);
    zox_gpu_bind_buffer_element(0);
    zox_gpu_bind_buffer_array(mesh.y);

    zox_gpu_enable_attribute_float2(vertex_shader_index);
    //glEnableVertexAttribArray(vertex_shader_index);
    // glVertexAttribPointer(vertex_shader_index, 2, GL_FLOAT, GL_FALSE, 4 * float_per_data, (GLvoid*)(0 * sizeof(float)));

    zox_gpu_set_buffer_array(combined_verts, floats_length * 4);

    // glBufferData(GL_ARRAY_BUFFER, floats_length * 4, combined_verts, GL_STATIC_DRAW);

    zox_gpu_bind_buffer_array(0);
}
