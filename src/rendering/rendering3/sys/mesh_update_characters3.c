/*void opengl_upload_mesh_colors(
    guint2 mesh_buffer,
    guint color_buffer,
    const int *indicies,
    int indicies_length,
    const float3 *verts,
    const color_rgb *color_rgbs,
    uint verts_length)
{
    zox_gpu_bind_buffer_element(mesh_buffer.x);
    zox_gpu_set_buffer_element(indicies, indicies_length * sizeof(int));
    zox_gpu_bind_buffer_array(mesh_buffer.y);
    zox_gpu_set_buffer_array(verts, verts_length * sizeof(float3));
    zox_gpu_bind_buffer_array(color_buffer);
    zox_gpu_set_buffer_array(color_rgbs, verts_length * sizeof(color_rgb));
    zox_gpu_bind_buffer_element(0);
    zox_gpu_bind_buffer_array(0);
}*/

zox_sys2(MeshUpdateCharacters3DSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshIndicies);
    zox_sys_in(MeshVertices);
    zox_sys_in(MeshColorRGBs);
    zox_sys_out(MeshGPULink);
    zox_sys_out(ColorsGPULink);
    zox_sys_out(MeshRenderCount);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MeshVertices, verts);
        zox_sys_i(MeshColorRGBs, colors);
        zox_sys_o(MeshGPULink, gpu_mesh);
        zox_sys_o(ColorsGPULink, gpu_colors);
        zox_sys_o(MeshRenderCount, count);
        // Spawn new GPU Buffers
        if (!gpu_mesh->value.x ||
            !gpu_mesh->value.y ||
            !gpu_colors->value)
        {
            // zox_loge("Character GPU Links broken [%s]", zox_getn(e));
            continue;
        }
        zox_gpu_element_buffer(
            gpu_mesh->value.x,
            indicies->length,
            sizeof(int),
            indicies->value);
        zox_gpu_array_buffer(
            gpu_mesh->value.y,
            verts->length,
            sizeof(float3),
            verts->value);
        zox_gpu_array_buffer(
            gpu_colors->value,
            verts->length,
            sizeof(color_rgb),
            colors->value);
        count->value = indicies->length;
        zox_remove(e, MeshDirty);
        if (dbg_log) {
            zox_log("Uploaded Mesh3 Colored [%s] [%i : %i]",
                zox_sys_e_name,
                verts->length,
                colors->length);
        }
    }
} zox_sys_end(MeshUpdateCharacters3DSystem);
