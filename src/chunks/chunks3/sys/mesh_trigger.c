// should i grab neighbor states instead, or should i create setters?
//  probably grab them, its faster to write in my systems
// When: VoxelNodeDirty is zox_dirty_active
zox_sys2(ColoredChunkMeshTriggerSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNodeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNodeDirty, dirty);
        // if node dirty, or for now, if node depth dirty... wait we dont want to double up
        if (dirty->value == zox_dirty_active) {
            zox_setv(e, BuildMesh, zox_build_chunk_mesh_run);
        }
    }
} zox_sys_end(ColoredChunkMeshTriggerSystem);
