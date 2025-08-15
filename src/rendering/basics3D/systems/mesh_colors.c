void MeshColorsGpuSystem(iter *it) {

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
        zox_gpu_buffer(
            link->value,
            verts->length,
            sizeof(color_rgb),
            colors->value
        );

    }
} zoxd_system(MeshColorsGpuSystem)
