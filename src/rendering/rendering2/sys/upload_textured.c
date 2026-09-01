zox_sys2(MeshUVs2UploadSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(MeshIndicies);
    zox_sys_in(MeshVertices2D);
    zox_sys_in(MeshUVs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshGPULink, mesh_id);
        zox_sys_i(UvsGPULink, uvs_id);
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MeshVertices2D, verts);
        zox_sys_i(MeshUVs, uvs);
// #ifdef zox_safety_checks
        if (!mesh_id->value.x ||
            !mesh_id->value.y ||
            !uvs_id->value)
        {
            // zox_loge("GPU Links Invalid for Textured2D Mesh [%s]", zox_getn(e));
            // upload->value = 0;
            continue;
        }
// #endif
        zox_gpu_bind_buffer_element(mesh_id->value.x);
        zox_gpu_set_buffer_element(indicies->value, indicies->length * sizeof(int));
        zox_gpu_bind_buffer_array(mesh_id->value.y);
        zox_gpu_set_buffer_array(verts->value, verts->length * sizeof(float2));
        zox_gpu_bind_buffer_array(uvs_id->value);
        zox_gpu_set_buffer_array(uvs->value, uvs->length * sizeof(float2));
        zox_gpu_bind_buffer_element(0);
        zox_gpu_bind_buffer_array(0);
        zox_remove(e, MeshDirty);
        if (dbg_log) {
            entity parent = zox_get_parent(world, e);
            zox_log("Uploaded Mesh Textured2 [%s] Tris [%i] Verts [%i] Parent [%s]",
                zox_getn(e),
                indicies->length / 3,
                verts->length,
                zox_getn(parent));
        }
    }
} zox_sys_end(MeshUVs2UploadSystem);
