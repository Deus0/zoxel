// TODO: There is no delay so it might get skipped
/*void chunk_mesh_colors_trigger_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, BuildMesh);
        if (!zox_no_lights) {
            zox_add(e, BuildMeshColors);
        }
        if (dbg_log) {
            zox_log("Chunk [%s] is [zox_build_chunk_mesh_lights]", zox_getn(e));
        }
        //}
    }
    zox_sys_on_end();
} zoxd_system(chunk_mesh_colors_trigger_system);*/