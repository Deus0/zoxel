zox_sys2(MeshColorsGpuSystem) {
    zox_sys_begin();
    zox_sys_in(MeshColorsDirty);
    zox_sys_in(MeshColorRGBs);
    zox_sys_in(ColorsGPULink);
    zox_sys_in(MeshVertices);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(MeshColorsDirty, dirty);
        zox_sys_i(ColorsGPULink, link);
        zox_sys_i(MeshVertices, verts);
        zox_sys_i(MeshColorRGBs, colors);

        // Only update if marked dirty for upload
        if (dirty->value != zox_dirty_active) {
            continue;
        }

        // Skip if invalid buffer or missing data
        if (!link->value || !verts->length || !colors->value) {
            continue;
        }

        // Upload only the color data
        zox_gpu_array_buffer(
            link->value,
            verts->length,
            sizeof(color_rgb),
            colors->value
        );

        /*zox_sys_world();
        zox_sys_e();
        zox_log("updating colors %s", zox_get_name(e));*/
    }
} zox_sys_end(MeshColorsGpuSystem);
