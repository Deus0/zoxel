// should i grab neighbor states instead, or should i create setters?
//  probably grab them, its faster to write in my systems

// When: VoxelNodeDirty is zox_dirty_active
void Chunk3MeshTriggerSystem(iter *it) {
    zox_sys_begin();
    zox_sys_in(VoxelNodeDirty);
    zox_sys_out(ChunkMeshDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(VoxelNodeDirty, voxel_node_dirty);
        zox_sys_o(ChunkMeshDirty, chunk_mesh_dirty);

        if (chunk_mesh_dirty->value != zox_dirty_none) {
            continue;
        }

        // if node dirty, or for now, if node depth dirty... wait we dont want to double up
        if (voxel_node_dirty->value == zox_dirty_active) {
            chunk_mesh_dirty->value = zox_dirty_trigger;
            /*zox_sys_world();
            zox_sys_e();
            zox_log("triggering chunk mesh [%s]", zox_get_name(e));*/
        }
    }
} zoxd_system2(Chunk3MeshTriggerSystem);