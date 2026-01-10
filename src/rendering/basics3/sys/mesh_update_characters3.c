void opengl_upload_mesh_colors(uint2 mesh_buffer, uint color_buffer, const int *indicies, int indicies_length, const float3 *verts, const color_rgb *color_rgbs, int verts_length) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_buffer.x);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies_length * sizeof(int), indicies, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_buffer.y);
    glBufferData(GL_ARRAY_BUFFER, verts_length * sizeof(float3), verts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, verts_length * sizeof(color_rgb), color_rgbs, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#ifdef zoxel_catch_opengl_errors
    check_opengl_error("opengl_upload_mesh_colors");
#endif
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
            if (gpu_mesh->value.x != 0 && gpu_mesh->value.y != 0 && gpu_colors->value != 0) {
                clear_regular_buffer(&gpu_mesh->value.x);
                clear_regular_buffer(&gpu_mesh->value.y);
            }
            if (gpu_colors->value) clear_regular_buffer(&gpu_colors->value);
            continue;
        }
        if (gpu_mesh->value.x == 0 && gpu_mesh->value.y == 0) {
            gpu_mesh->value.x = spawn_gpu_generic_buffer();
            gpu_mesh->value.y = spawn_gpu_generic_buffer();
        }
        if (gpu_colors->value == 0) {
            gpu_colors->value = spawn_gpu_generic_buffer();
        }
        // zox_log(" + Uploading mesh [%i : %i]\n", meshVertices->length, colors->length)

        zox_gpu_element_buffer(
            gpu_mesh->value.x,
            indicies->length,
            sizeof(int),
            indicies->value
        );

        zox_gpu_array_buffer(
            gpu_mesh->value.y,
            verts->length,
            sizeof(float3),
            verts->value
        );

        zox_gpu_array_buffer(
            gpu_colors->value,
            verts->length,
            sizeof(color_rgb),
            colors->value
        );

        count->value = indicies->length;
    }
} zox_sys_end(MeshUpdateCharacters3DSystem);