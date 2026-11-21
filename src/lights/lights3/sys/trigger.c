// When Mesh Rebuilds, rebuild our colors too based on lights
// If queue empty + lightnode dirty, activate!
void chunk_colors_trigger_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LightNodeDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LightNodeDirty, lights_dirty);
        if (lights_dirty->value != zox_dirty_active) {
            continue;
        }
        // NOTE: was inverted (skipped its meshes, tagged its other children), so a chunk's own light
        //  changes only recolored its neighbors (tst block_vox_lights)
        entity meshes[zox_chunk_meshes_max];
        const uint count = get_chunk_meshes(world, e, meshes);
        for (uint j = 0; j < count; j++) {
            zox_add(meshes[j], BuildMeshColors);
        }
    }
    zox_sys_on_end();
} zoxd_system(chunk_colors_trigger_system);

// NOTE: Generates lights again when the neighbor lights update
void chunk_neighbor_light_trigger_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LightNodeDirty);
    zox_sys_in(ChunkNeighbors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LightNodeDirty, dirty);
        zox_sys_i(ChunkNeighbors, neighbors);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        for (byte j = 0; j < chunk_neighbors_length; j++) {
            entity neighbor = neighbors->value[j];
            if (!zox_valid(neighbor)) {
                continue;
            }
            entity meshes[zox_chunk_meshes_max];
            const uint count = get_chunk_meshes(world, neighbor, meshes);
            for (uint k = 0; k < count; k++) {
                zox_add(meshes[k], BuildMeshColors);
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(chunk_neighbor_light_trigger_system);

