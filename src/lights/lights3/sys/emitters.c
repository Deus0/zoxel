// a chunk's emitters light their sides once its voxels are in, generated or loaded (GenerateEmitterLight, added with
//  its sunlight, src/terrains/voxel/sys/generated.c): a loaded chunk's glow stones were dark, only edits and towns lit them
void emitter_light_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNode);
    zox_sys_in(NodeDepth);
    zox_sys_in(ChunkNeighbors);
    zox_sys_out(LightQueue);
    entity emission_realm = 0;
    byte emission[zox_light_solidity_length];
    byte3 emission_offset[zox_light_solidity_length];
    memset(emission, 0, zox_light_solidity_length);
    memset(emission_offset, 0, sizeof(emission_offset));
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNode, voxels);
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(ChunkNeighbors, neighbors);
        zox_sys_o(LightQueue, light_queue);
        update_light_emission(world, zox_get_link(world, e, BlockManagerLink), &emission_realm, emission, emission_offset);
        LightQueue* side_queues[6];
        fetch_neightbor_propogation_queues(world, neighbors, side_queues);
        const BlockDatas* datas = zox_has(e, BlockDatas) ? zox_get(e, BlockDatas) : NULL;
        emitters_light_chunk(voxels, datas, depth->value, emission, emission_offset, light_queue, side_queues);
        zox_remove(e, GenerateEmitterLight);
    }
    zox_sys_on_end();
} zoxd_system(emitter_light_system);
