// should i grab neighbor states instead, or should i create setters?
//  probably grab them, its faster to write in my systems
// When: VoxelNodeDirty is zox_dirty_active
zox_sys2(ColoredChunkMeshTriggerSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_add(e, BuildMesh);
        if (dbg_log) {
            zox_log("Chunk Triggered to BuildMesh [%s]", zox_sys_e_name);
        }
    }
} zox_sys_end(ColoredChunkMeshTriggerSystem);
