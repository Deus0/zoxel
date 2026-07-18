zox_sys2(MeshColorsGpuSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(MeshColorRGBs);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(MeshVertices);
    zox_sys_out(MeshColorsDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ColorsGPULink, link);
        zox_sys_i(MeshVertices, verts);
        zox_sys_i(MeshColorRGBs, colors);
        zox_sys_o(MeshColorsDirty, upload);
        // Only update if marked dirty for upload
        if (!upload->value) {
            continue;
        }
        if (!link->value) {
            zox_loge("GPU Links Invalid on Chunk Mesh [%s]", zox_getn(e));
            continue;
        }
        // Skip if invalid buffer or missing data
        if (!verts->length || !colors->value) {
            if (dbg_log) {
                zox_log("Mesh had no verts colors %s", zox_get_name(e));
            }
            // continue;
        }
        // Upload only the color data
        zox_gpu_array_buffer(link->value, verts->length, sizeof(color_rgb), colors->value);
        upload->value = 0;
        if (dbg_log) {
            zox_log("updating colors %s", zox_get_name(e));
        }
    }
} zox_sys_end(MeshColorsGpuSystem);
