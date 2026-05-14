typedef struct {
    VoxelNode *chunk;
    int3 octree_position;
    byte depth;
    byte max_depth;
} NodeDelveData;

typedef struct {
    entity chunk;
    const entity *blocks;
    const entity *models;
    const entity *block_prefabs;
    const byte *block_vox_offsets;
    byte blocks_length;
    float3 chunk_positionf;
    float chunk_scalev;
    float tscale;
    byte render_disabled;
    byte render_depth;
} UpdateBlockEntities;

// Our main spawn function
void spawned_block_vox(ecs *world, spawned_block_data* data) {
    if (!zox_has(data->block, BlockPrefabLink)) {
        return;
    }
    if (!data->node) {
        zox_log_error("null node in [spawned_block_vox]")
        return;
    }
    // gett block data
    zox_geter_value(data->block, BlockPrefabLink, entity, prefab);
    zox_geter_value_safe(data->block, ModelLink, entity, vox);
    SpawnBlockVox spawn_data = {
        .prefab = prefab,
        .vox = vox,
        .block_index = data->block_index,
        .positionl = data->positionl,
        .positionv = data->positionv,
        .positionf = data->positionf,
        .scale = data->scale,
        .render_depth = data->render_depth,
        .render_disabled = data->render_disabled,
    };
    entity e2;
    if (zox_has(prefab, BlockVox)) {
        e2 = spawn_block_vox(world, spawn_data);
    } else if (zox_has(prefab, RendererInstance)) {
        e2 = spawn_block_vox_instanced(world, spawn_data);
    } else {
        return;
    }
    link_node_VoxelNode(data->node, e2);
    /*zox_log("+ Placing Block [%s]: linked: [%i]", zox_get_name(data->block), data->node->linked)
    zox_log("   - local [%ix%ix%i] ", spawn_data.positionl.x, spawn_data.positionl.y, spawn_data.positionl.z)
    zox_log("   - global [%ix%ix%i] ", spawn_data.positionv.x, spawn_data.positionv.y, spawn_data.positionv.z)
    zox_log("   - real [%fx%fx%f] ", spawn_data.positionf.x, spawn_data.positionf.y, spawn_data.positionf.z)*/
    // spawn_line3(world, spawn_data.positionf, float3_add(spawn_data.positionf, (float3) { 0, 2, 0 }), 2, 3);
}


void spawn_vodes_dive(ecs *world, const UpdateBlockEntities *data, NodeDelveData *delve_data) {
    VoxelNode *node = delve_data->chunk;
    if (!node) {
        return;
    }
    if (delve_data->depth != delve_data->max_depth) {
        // did more
        int3 octree_position = delve_data->octree_position;
        int3_multiply_int_p(&octree_position, 2);
        if (has_children_VoxelNode(node)) {
            VoxelNode* kids = get_children_VoxelNode(node);
            for (byte i = 0; i < octree_length; i++) {
                NodeDelveData delve_data_child = {
                    .chunk = &kids[i],
                    .octree_position = int3_add(octree_position, octree_positions[i]),
                    .depth = delve_data->depth + 1,
                    .max_depth = delve_data->max_depth
                };
                spawn_vodes_dive(world, data, &delve_data_child);
            }
        }
        return;
    }
    // air returns!
    if (!node->value) {
        return;
    }
    // check if out of bounds
    byte block_index = node->value - 1;
    if (block_index >= data->blocks_length) {
        zox_log_error("block_index out of bounds %i of %i", block_index, data->blocks_length)
        return;
    }
    // Remove and return if not a World Block
    entity block_prefab = data->block_prefabs[block_index];
    if (!block_prefab) {
        return;
    }
    // + spawn block vox
    // if exists already, shouldn't we check if is the same block vox type?
    // if exists, and is same type, return!
    // read lock here?
    if (is_linked_VoxelNode(node)) {
        return;
    }
    float mesh_scale = data->chunk_scalev;
    // TODO: If loaded vox model, we need to scale based on the mesh we are using
    byte3 positionl = int3_to_byte3(delve_data->octree_position);
    int3 positionv = delve_data->octree_position;
    float3 positionf = float3_from_int3(positionv);
    float3_scale_p(&positionf, data->tscale);
    float3_add_float3_p(&positionf, data->chunk_positionf);
    float3_add_float3_p(&positionf, float3_single(-mesh_scale * 0.5f));
    if (node->value && !is_linked_VoxelNode(node)) {
        zox_geter(data->chunk, ChunkPosition, cposition);
        zox_geter_value(data->chunk, NodeDepth, byte, node_depth);
        int chunk_length = powers_of_two[node_depth];
        int3 chunk_dimensions = int3_single(chunk_length);
        int3 cpositionv = get_chunk_positionv(cposition->value, chunk_dimensions);
        int3 positionv = int3_add(positionv, cpositionv);
        // spawn node entity here!
        byte block_index = node->value - 1;
        if (block_index >= data->blocks_length) {
            zox_log_error("voxel [%i] is out of range [%i]", block_index, data->blocks_length)
            return;
        }
        entity block = data->blocks[block_index];
        spawned_block_data spawned_data = (spawned_block_data) {
            .chunk = data->chunk,
            .node = node,
            .block_index = block_index,
            .block = block,
            .positionl = positionl,
            .positionv = positionv,
            .positionf = positionf,
            .scale = mesh_scale,
            .render_disabled = data->render_disabled,
            .render_depth = data->render_depth,
        };
        spawned_block_vox(world, &spawned_data);
        run_hook_spawned_block(world, &spawned_data);
    }
}


void spawn_vodes(ecs *world, entity e, entity terrain, byte render_depth, byte rdisabled, VoxelNode *chunk, byte max_depth, float3 positionf, float chunk_scalev, float tscale) {
    zox_geter_value(terrain, RealmLink, entity, realm);
    if (!zox_valid(realm)) {
        return;
    }
    zox_geter(realm, BlockLinks, blocks);
    byte blocks_length = blocks->length;
    if (!blocks_length) {
        return;
    }
    entity blocksarr[blocks_length];
    entity models[blocks_length];
    entity block_prefabs[blocks_length];
    byte block_vox_offsets[blocks_length];
    zero_memory(models, blocks_length, entity);
    zero_memory(block_prefabs, blocks_length, entity);
    zero_memory(block_vox_offsets, blocks_length, byte);
    for (int j = 0; j < blocks_length; j++) {
        entity block = blocks->value[j];
        if (!zox_valid(block)) {
            continue;
        }
        blocksarr[j] = block;
        if (zox_gett_value(block, BlockModel) == zox_block_vox) {
            models[j] = zox_gett_value(block, ModelLink);
            if (zox_has(block, BlockVoxOffset)) {
                block_vox_offsets[j] = zox_get_value(block, BlockVoxOffset);
            }
        }
        if (zox_has(block, BlockPrefabLink)) {
            block_prefabs[j] = zox_get_value(block, BlockPrefabLink);
        }
    }
    // why we do this?
    positionf = float3_add(positionf, float3_single(tscale));
    UpdateBlockEntities data = {
        .chunk_scalev = chunk_scalev,
        .tscale = tscale,
        .chunk = e,
        .blocks_length = blocks_length,
        .blocks = blocksarr, // metas
        .block_prefabs = block_prefabs,
        .models = models,
        .block_vox_offsets = block_vox_offsets,
        .chunk_positionf = positionf,
        .render_depth = render_depth,
        .render_disabled = rdisabled,
    };
    NodeDelveData delve_data = {
        .chunk = chunk,
        .depth = 0,
        .max_depth = max_depth
    };
    spawn_vodes_dive(world, &data, &delve_data);
}

// Triggers: [VoxelNodeDirty] + [RenderDistanceDirty]
zox_sys2(VodesSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNodeDirty);
    zox_sys_in(RenderDistanceDirty);
    zox_sys_in(VoxLink);
    zox_sys_in(NodeDepth);
    zox_sys_in(RenderDisabled);
    zox_sys_in(RenderDepth);
    zox_sys_in(RenderDistance);
    zox_sys_in(Position3D);
    zox_sys_in(BlockScale);
    zox_sys_out(VoxelNode);
    zox_sys_out(BlocksSpawned);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNodeDirty, voxelNodeDirty);
        zox_sys_i(RenderDistanceDirty, renderDistanceDirty);
        zox_sys_i(VoxLink, terrain);
        zox_sys_i(NodeDepth, nodeDepth);
        zox_sys_i(RenderDisabled, rdisabled);
        zox_sys_i(RenderDepth, renderDepth);
        zox_sys_i(RenderDistance, renderDistance);
        zox_sys_i(Position3D, position);
        zox_sys_i(BlockScale, scale);
        zox_sys_o(VoxelNode, node);
        zox_sys_o(BlocksSpawned, spawned);
        // either voxel node is dirty, or we are spawning for first time based on distance changes
        byte is_first_time = voxelNodeDirty->value == zox_dirty_active;
        byte is_dirty = (!spawned->value && renderDistanceDirty->value == zox_dirty_active);
        if (!is_first_time && !is_dirty) {
            continue;
        }
        //  base off render distance
        zox_geter_value(terrain->value, NodeDepth, byte, terrain_depth);
        zox_geter_value(terrain->value, BlockScale, float, tscale);
        byte can_spawn_vodes = renderDepth->value == terrain_depth;
        if (!can_spawn_vodes) {
            continue;
        }
        byte rdepth = camera_distance_to_block_vox_depth(renderDistance->value);
        write_lock_VoxelNode(node);
        spawn_vodes(world, e, terrain->value, rdepth, rdisabled->value, node, nodeDepth->value, position->value, scale->value, tscale);
        write_unlock_VoxelNode(node);
        spawned->value = 1;
    }
} zox_sys_end(VodesSpawnSystem);
