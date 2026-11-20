// gives new meshes their gpu links; none while the gpu's lost (a dispose: android's context, a minimise), the
// restore leaves links that held nothing for these to make after (a mesh appearing then made them with no gpu,
// tst gpu_restore_game failed 2 runs in 6)
byte dbg_initialize_mesh = 0;

void initialize_mesh_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0 || dbg_initialize_mesh;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(MeshGPULink);
    for (int i = 0; rendering && i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(MeshGPULink, mesh);
        mesh->value = spawn_gpu_mesh_buffers();
        if (dbg_log) {
            zox_log("Mesh Initialized on GPU [%s]", zox_getn(e));
        }
    }
    zox_sys_on_end();
} zoxd_system(initialize_mesh_system);

void initialize_mesh_uvs_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = dbg_initialize_mesh || 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(UvsGPULink);
    for (int i = 0; rendering && i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(UvsGPULink, uvs);
        uvs->value = zox_gpu_create_buffer();
        if (dbg_log) {
            zox_log("Mesh UVs Initialized on GPU [%s]", zox_getn(e));
        }
    }
    zox_sys_on_end();
} zoxd_system(initialize_mesh_uvs_system);

void initialize_mesh_colors_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = dbg_initialize_mesh || 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(ColorsGPULink);
    for (int i = 0; rendering && i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(ColorsGPULink, colors);
        colors->value = zox_gpu_create_buffer();
        if (dbg_log) {
            zox_log("Mesh Colors Initialized on GPU [%s]", zox_getn(e));
        }
    }
    zox_sys_on_end();
} zoxd_system(initialize_mesh_colors_system);

void initialize_mesh_texture_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0 || dbg_initialize_mesh;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(TextureGPULink);
    for (int i = 0; rendering && i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(TextureGPULink, texture);
        texture->value = spawn_gpu_texture_buffer();
        zox_gpu_clear_texture_rgba(texture->value);
        if (dbg_log) {
            zox_log("Mesh Texture Initialized on GPU [%s] -> [%i]",
                zox_getn(e),
                texture->value);
        }
    }
    zox_sys_on_end();
} zoxd_system(initialize_mesh_texture_system);

