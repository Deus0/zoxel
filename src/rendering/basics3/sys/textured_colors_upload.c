zox_sys2(MeshColorsGpuSystem) {
    // TODO: Just use mesh colors dirty and use other flag
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ColorsGPULink);
    zox_sys_in(MeshColorRGBs);
    //zox_sys_in(MeshColorsDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ColorsGPULink, link);
        zox_sys_i(MeshColorRGBs, colors);
        if (!link->value) {
            continue;
        }
        // Skip if invalid buffer or missing data
        /*if (!verts->length || !colors->value) {
            if (dbg_log) {
                zox_log("Mesh had no verts colors %s", zox_getn(e));
            }
            // continue;
        }*/
        // Upload only the color data
        zox_gpu_array_buffer(
            link->value,
            colors->length,
            sizeof(color_rgb),
            colors->value);
        zox_add(e, MeshColorsBuilt);
        zox_remove(e, MeshColorsDirty);
        if (dbg_log) {
            zox_log("updating colors %s", zox_getn(e));
        }
    }
} zox_sys_end(MeshColorsGpuSystem);
