// When Mesh Rebuilds, rebuild our colors too based on lights
void MeshColorsTriggerSystem(iter *it) {

    zox_sys_begin();
    zox_sys_in(ChunkMeshDirty);
    zox_sys_in(SunlightDirty);
    zox_sys_out(MeshColorsGenerate);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(ChunkMeshDirty, mesh_dirty);
        zox_sys_i(SunlightDirty, sunlight_dirty);
        zox_sys_o(MeshColorsGenerate, mesh_colors_generate);

        if (mesh_dirty->value == chunk_dirty_state_end || sunlight_dirty->value == zox_dirty_active) {
            mesh_colors_generate->value = zox_dirty_trigger;
        }

    }
} zoxd_system2(MeshColorsTriggerSystem);