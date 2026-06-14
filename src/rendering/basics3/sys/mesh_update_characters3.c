void opengl_upload_mesh_colors(uint2 mesh_buffer, uint color_buffer, const int *indicies, int indicies_length, const float3 *verts, const color_rgb *color_rgbs, int verts_length) {
    zox_gpu_bind_buffer_element(mesh_buffer.x);
    zox_gpu_set_buffer_element(indicies, indicies_length * sizeof(int));
    zox_gpu_bind_buffer_array(mesh_buffer.y);
    zox_gpu_set_buffer_array(verts, verts_length * sizeof(float3));
    zox_gpu_bind_buffer_array(color_buffer);
    zox_gpu_set_buffer_array(color_rgbs, verts_length * sizeof(color_rgb));
    zox_gpu_bind_buffer_element(0);
    zox_gpu_bind_buffer_array(0);
}

zox_sys2(MeshUpdateCharacters3DSystem) {
    zox_sys_begin();
    zox_sys_in(MeshIndicies);
    zox_sys_in(MeshVertices);
    zox_sys_in(MeshColorRGBs);
    zox_sys_in(MeshDirty);
    zox_sys_out(MeshGPULink);
    zox_sys_out(ColorsGPULink);
    zox_sys_out(MeshIndiciesGpu);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(MeshDirty, dirty);
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MeshVertices, verts);
        zox_sys_i(MeshColorRGBs, colors);
        zox_sys_o(MeshGPULink, gpu_mesh);
        zox_sys_o(ColorsGPULink, gpu_colors);
        zox_sys_o(MeshIndiciesGpu, count);
        if (dirty->value != mesh_state_upload) {
            continue;
        }
        if (indicies->length == 0) {
            // clear mesh and colors buffer if zero again
            zox_gpu_dispose_buffer(gpu_mesh->value.x);
            zox_gpu_dispose_buffer(gpu_mesh->value.y);
            zox_gpu_dispose_buffer(gpu_colors->value);
            gpu_mesh->value.x = 0;
            gpu_mesh->value.y = 0;
            gpu_colors->value = 0;
            count->value = 0;
            continue;
        }
        // Spawn new GPU Buffers
        if (!gpu_mesh->value.x && !gpu_mesh->value.y) {
            gpu_mesh->value.x = zox_gpu_create_buffer();
            gpu_mesh->value.y = zox_gpu_create_buffer();
        }
        if (!gpu_colors->value) {
            gpu_colors->value = zox_gpu_create_buffer();
        }
        // zox_log(" + Uploading mesh [%i : %i]\n", meshVertices->length, colors->length)
        zox_gpu_element_buffer(gpu_mesh->value.x, indicies->length, sizeof(int), indicies->value);
        zox_gpu_array_buffer(gpu_mesh->value.y, verts->length, sizeof(float3), verts->value);
        zox_gpu_array_buffer(gpu_colors->value, verts->length, sizeof(color_rgb), colors->value);
        count->value = indicies->length;
    }
} zox_sys_end(MeshUpdateCharacters3DSystem);
