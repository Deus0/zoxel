zox_sys2(MeshDirtyRestoreSystem) {
    zox_sys_begin()
    zox_sys_out(MeshDirty)
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(MeshDirty, meshDirty)
        meshDirty->value = mesh_state_trigger;
    }
} zox_sys_end(MeshDirtyRestoreSystem);
