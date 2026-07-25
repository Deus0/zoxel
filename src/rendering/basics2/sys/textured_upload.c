zox_sys2(Mesh2DUvsUpdateSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshDirty);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(MeshIndicies);
    zox_sys_in(MeshVertices2D);
    zox_sys_in(MeshUVs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshDirty, dirty);
        zox_sys_i(MeshGPULink, mesh_id);
        zox_sys_i(UvsGPULink, uvs_id);
        zox_sys_i(MeshIndicies, indicies);
        zox_sys_i(MeshVertices2D, verts);
        zox_sys_i(MeshUVs, uvs);
        if (dirty->value != mesh_state_upload) {
            continue;
        }
        if (!indicies->length | !mesh_id->value.x || !mesh_id->value.y | !uvs->value) {
            // not every layout element has a mesh!
            // zox_sys_world()
            // zox_log_error("[%s] uploaded mesh2D_uvs failed due to [%s]", zox_get_name(it->entities[i]), !meshIndicies->length ? "no mesh" : "no gpu links")
            continue;
        }
        zox_gpu_bind_buffer_element(mesh_id->value.x);
        zox_gpu_set_buffer_element(indicies->value, indicies->length * sizeof(int));
        zox_gpu_bind_buffer_array(mesh_id->value.y);
        zox_gpu_set_buffer_array(verts->value, verts->length * sizeof(float2));
        zox_gpu_bind_buffer_array(uvs_id->value);
        zox_gpu_set_buffer_array(uvs->value, uvs->length * sizeof(float2));
        zox_gpu_bind_buffer_element(0);
        zox_gpu_bind_buffer_array(0);
        if (dbg_log) {
            zox_log("Mesh 2D Uploaded [%s]", zox_getn(e));
        }
    }
} zox_sys_end(Mesh2DUvsUpdateSystem);
