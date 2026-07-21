/*zox_sys2(Element3MeshSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(InitializeEntity);
    // zox_sys_out(MeshGPULink);
    zox_sys_out(UvsGPULink);
    zox_sys_out(ColorsGPULink);
    zox_sys_out(TextureGPULink);
    zox_sys_out(MeshDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(InitializeEntity, initialize);
        // zox_sys_o(MeshGPULink, meshGPULink);
        zox_sys_o(UvsGPULink, uvsGPULink);
        zox_sys_o(ColorsGPULink, colorsGPULink);
        zox_sys_o(TextureGPULink, textureGPULink);
        zox_sys_o(MeshDirty, mesh_dirty);
        if (initialize->value != zox_dirty_active) {
            continue;
        }
        // meshGPULink->value = spawn_gpu_mesh_buffers();
        textureGPULink->value = spawn_gpu_texture_buffer();
        uvsGPULink->value = zox_gpu_create_buffer();
        colorsGPULink->value = zox_gpu_create_buffer();
        mesh_dirty->value = mesh_state_trigger;
        initialize->value = 0;
        if (dbg_log) {
            zox_log("Element Mesh (3D) Initialized [%s]", zox_getn(e));
        }
    }
} zox_sys_end(Element3MeshSystem);*/
