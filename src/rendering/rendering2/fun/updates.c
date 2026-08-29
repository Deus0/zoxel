// const byte isDebugRenderMaterial2DSystem = 0;

void set_gpu_mesh2D(guint2 mesh, const int *indicies, int indicies_length, const float2 *verts, int verts_length) {
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
    zox_gpu_set_buffer_element(indicies, indicies_length * 4);
    zox_gpu_bind_buffer_element(0);
    zox_gpu_bind_buffer_array(mesh.y);
    zox_gpu_enable_attribute_float2(vertex_shader_index);
    zox_gpu_set_buffer_array(combined_verts, floats_length * 4);
    zox_gpu_bind_buffer_array(0);
}
