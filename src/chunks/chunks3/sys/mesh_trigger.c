// should i grab neighbor states instead, or should i create setters?
//  probably grab them, its faster to write in my systems
// When: VoxelNodeDirty is zox_dirty_active
zox_sys2(Chunk3MeshTriggerSystem) {
    zox_sys_begin();
    zox_sys_in(VoxelNodeDirty);
    zox_sys_out(ChunkMeshDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(VoxelNodeDirty, vdirty);
        zox_sys_o(ChunkMeshDirty, cdirty);
        // if node dirty, or for now, if node depth dirty... wait we dont want to double up
        if (vdirty->value == zox_dirty_active) {
            cdirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(Chunk3MeshTriggerSystem);
