// When Mesh Rebuilds, rebuild our colors too based on lights
// If queue empty + lightnode dirty, activate!

void MeshColorsTriggerSystem(iter *it) {
    zox_sys_begin();
    zox_sys_in(LightQueue);
    zox_sys_in(DarkQueue);
    zox_sys_in(ChunkMeshDirty);
    zox_sys_in(SunlightDirty);
    zox_sys_in(LightNodeDirty);
    zox_sys_out(MeshColorsGenerate);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LightQueue, light_queue);
        zox_sys_i(DarkQueue, dark_queue);
        zox_sys_i(ChunkMeshDirty, chunk_mesh_dirty);
        zox_sys_i(SunlightDirty, sunlight_dirty);
        zox_sys_i(LightNodeDirty, light_node_dirty);
        zox_sys_o(MeshColorsGenerate, mesh_colors_generate);

        if (light_queue->count || dark_queue->count) {
            // continue;
        }

        /*if (chunk_mesh_dirty->value) {
            zox_sys_world();
            zox_sys_e();
            zox_log("MeshColorsTriggerSystem %s", zox_get_name(e));
        }*/

        if (chunk_mesh_dirty->value == zox_dirty_active ||
            sunlight_dirty->value == zox_dirty_active ||
            light_node_dirty->value == zox_dirty_active
        ) {
            mesh_colors_generate->value = zox_dirty_trigger;
        }

    }
} zoxd_system2(MeshColorsTriggerSystem);