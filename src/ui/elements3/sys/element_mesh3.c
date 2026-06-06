zox_sys2(Element3DMeshSystem) {
    zox_sys_begin();
    zox_sys_out(InitializeEntity);
    zox_sys_out(MeshDirty);
    zox_sys_out(MeshGPULink);
    zox_sys_out(UvsGPULink);
    zox_sys_out(ColorsGPULink);
    zox_sys_out(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(InitializeEntity, initializeElement);
        zox_sys_o(MeshDirty, mesh_dirty);
        zox_sys_o(MeshGPULink, meshGPULink);
        zox_sys_o(TextureGPULink, textureGPULink);
        zox_sys_o(UvsGPULink, uvsGPULink);
        zox_sys_o(ColorsGPULink, colorsGPULink);

        if (!initializeElement->value || mesh_dirty->value) {
            continue;
        }

        meshGPULink->value = spawn_gpu_mesh_buffers();
        textureGPULink->value = spawn_gpu_texture_buffer();
        uvsGPULink->value = zox_gpu_create_buffer();
        colorsGPULink->value = zox_gpu_create_buffer();

        mesh_dirty->value = mesh_state_trigger;
        initializeElement->value = 0;
        zox_log_elements3D("+ updated mesh for element3D [%lu]", it->entities[i]);
    }
} zox_sys_end(Element3DMeshSystem);
