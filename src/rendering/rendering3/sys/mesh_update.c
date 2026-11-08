zox_sys2(MeshUpdateSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshIndicies);
    zox_sys_in(MeshVertices);
    zox_sys_in(MeshGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshGPULink, gpu_mesh);
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MeshVertices, verts);
        // Spawn new GPU Buffers
        if (!gpu_mesh->value.x ||
            !gpu_mesh->value.y)
        {
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
        zox_remove(e, MeshDirty);
        if (dbg_log) {
            zox_log("Uploaded Mesh3 Basic [%s] [%i]",
                zox_sys_e_name,
                verts->length);
            if (check_opengl_error_unlogged()) {
                zox_loge("(MeshUpdate) GL error uploading [%s]", zox_sys_e_name);
            }
            // what actually landed on the GPU
            GLint ebo_size = 0;
            GLint vbo_size = 0;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_mesh->value.x);
            glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &ebo_size);
            glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh->value.y);
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vbo_size);
            zox_log("Uploaded Mesh3 Basic [%s] buffers [%ix%i]",
                    zox_sys_e_name, gpu_mesh->value.x, gpu_mesh->value.y);
            zox_log("   - GPU sizes: indices [%i / %i bytes] vertices [%i / %i bytes]",
                    ebo_size, indicies->length * (int) sizeof(int),
                    vbo_size, verts->length * (int) sizeof(float3));
            if (indicies->length && indicies->value) {
                int max_index = 0;
                for (int j = 0; j < indicies->length; j++) {
                    if (indicies->value[j] > max_index) max_index = indicies->value[j];
                }
                zox_log("   - Indices [%i] max index [%i] (must be < %i vertices)",
                        indicies->length, max_index, verts->length);
            }
            if (verts->length && verts->value) {
                float3 lo = verts->value[0];
                float3 hi = verts->value[0];
                for (int j = 1; j < verts->length; j++) {
                    lo.x = fminf(lo.x, verts->value[j].x); hi.x = fmaxf(hi.x, verts->value[j].x);
                    lo.y = fminf(lo.y, verts->value[j].y); hi.y = fmaxf(hi.y, verts->value[j].y);
                    lo.z = fminf(lo.z, verts->value[j].z); hi.z = fmaxf(hi.z, verts->value[j].z);
                }
                zox_log("   - Bounds [%.2f %.2f %.2f] to [%.2f %.2f %.2f]",
                        lo.x, lo.y, lo.z, hi.x, hi.y, hi.z);
            }
        }
    }
} zox_sys_end(MeshUpdateSystem);
