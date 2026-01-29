// When Mesh Rebuilds, rebuild our colors too based on lights
// If queue empty + lightnode dirty, activate!

zox_sys2(MeshColorsTriggerSystem) {
    if (disable_lights) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(LightQueue);
    zox_sys_in(DarkQueue);
    zox_sys_in(VoxelNodeDirty);
    zox_sys_in(ChunkMeshDirty);
    zox_sys_in(SunlightDirty);
    zox_sys_in(LightNodeDirty);
    zox_sys_out(MeshColorsGenerate);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LightQueue, lqueue);
        zox_sys_i(DarkQueue, dqueue);
        zox_sys_i(VoxelNodeDirty, vdirty);
        zox_sys_i(ChunkMeshDirty, cdirty);
        zox_sys_i(SunlightDirty, sdirty);
        zox_sys_i(LightNodeDirty, ldirty);
        zox_sys_o(MeshColorsGenerate, generate);

        if (lqueue->count || dqueue->count || vdirty->value == zox_dirty_trigger || vdirty->value == zox_dirty_active) {
            continue;
        }

        if (cdirty->value == zox_dirty_active) {
            // && !sdirty->value && !ldirty->value) {
            generate->value = zox_dirty_trigger;
        }

        else if (sdirty->value == zox_dirty_active ||
            ldirty->value == zox_dirty_active
        ) {
            generate->value = zox_dirty_trigger;
        }

    }
} zox_sys_end(MeshColorsTriggerSystem);