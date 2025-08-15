// When Mesh Rebuilds, rebuild our colors too based on lights
void MeshColorsTriggerSystem(iter *it) {

    zox_sys_begin();
    zox_sys_in(ChunkMeshDirty);
    zox_sys_out(MeshColorsGenerate);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(ChunkMeshDirty, mesh_dirty);
        zox_sys_o(MeshColorsGenerate, update);

        if (mesh_dirty->value == chunk_dirty_state_end) {
            update->value = zox_dirty_trigger; // for now just put here
        }

    }
} zoxd_system2(MeshColorsTriggerSystem);