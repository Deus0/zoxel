byte dbg_initialize_mesh = 0;

zox_sys2(InitializeMeshSystem) {
    byte dbg_log = dbg_initialize_mesh || 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Initialize);
    zox_sys_out(MeshGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Initialize, initialize);
        zox_sys_o(MeshGPULink, mesh);
        if (initialize->value != zox_dirty_active) {
            continue;
        }
        mesh->value = spawn_gpu_mesh_buffers();
        if (dbg_log) {
            zox_log("Mesh Initialized on GPU [%s]", zox_getn(e));
        }
    }
} zox_sys_end(InitializeMeshSystem);

zox_sys2(InitializeMeshUvsSystem) {
    byte dbg_log = dbg_initialize_mesh || 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Initialize);
    zox_sys_out(UvsGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Initialize, initialize);
        zox_sys_o(UvsGPULink, uvs);
        if (initialize->value != zox_dirty_active) {
            continue;
        }
        uvs->value = zox_gpu_create_buffer();
        if (dbg_log) {
            zox_log("Mesh UVs Initialized on GPU [%s]", zox_getn(e));
        }
    }
} zox_sys_end(InitializeMeshUvsSystem);

zox_sys2(InitializeMeshColorsSystem) {
    byte dbg_log = dbg_initialize_mesh || 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Initialize);
    zox_sys_out(ColorsGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Initialize, initialize);
        zox_sys_o(ColorsGPULink, colors);
        if (initialize->value != zox_dirty_active) {
            continue;
        }
        colors->value = zox_gpu_create_buffer();
        if (dbg_log) {
            zox_log("Mesh Colors Initialized on GPU [%s]", zox_getn(e));
        }
    }
} zox_sys_end(InitializeMeshColorsSystem);

zox_sys2(InitializeMeshTextureSystem) {
    byte dbg_log = dbg_initialize_mesh || 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Initialize);
    zox_sys_out(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Initialize, initialize);
        zox_sys_o(TextureGPULink, texture);
        if (initialize->value != zox_dirty_active) {
            continue;
        }
        texture->value = spawn_gpu_texture_buffer();
        if (dbg_log) {
            zox_log("Mesh Texture Initialized on GPU [%s]", zox_getn(e));
        }
    }
} zox_sys_end(InitializeMeshTextureSystem);

