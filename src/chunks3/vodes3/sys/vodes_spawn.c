typedef struct {
    VoxelNode *chunk;
    int3 octree_position;
    byte depth;
    const byte max_depth;
} NodeDelveData;

typedef struct {
    const entity chunk;
    const entity *blocks;
    const entity *models;
    const entity *block_prefabs;
    const byte *block_vox_offsets;
    const byte blocks_length;
    const float3 chunk_positionf;
    const float chunk_scalev;
    const float terrain_scalev;
    const byte render_disabled;
    const byte render_depth;
} UpdateBlockEntities;

void spawn_vodes_dive(
    ecs *world,
    const UpdateBlockEntities *data,
    NodeDelveData *delve_data
) {
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
    // cheeck if out of bounds
    const byte block_index = node->value - 1;
    if (block_index >= data->blocks_length) {
        zox_log_error("block_index out of bounds %i of %i", block_index, data->blocks_length)
        return;
    }

    // Remove and return if not a World Block
    const entity block_prefab = data->block_prefabs[block_index];
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

    byte3 positionl = int3_to_byte3(delve_data->octree_position);
    int3 positionv = delve_data->octree_position;
    float3 positionf = float3_from_int3(positionv);
    float3_scale_p(&positionf, data->terrain_scalev);
    float3_add_float3_p(&positionf, data->chunk_positionf);
    float3_add_float3_p(&positionf, float3_single(-data->chunk_scalev * 0.5f));

    if (node->value && !is_linked_VoxelNode(node)) {
        zox_geter(data->chunk, ChunkPosition, chunkPosition)
        zox_geter_value(data->chunk, NodeDepth, byte, node_depth)
        const int chunk_length = powers_of_two[node_depth];
        const int3 chunk_dimensions = int3_single(chunk_length);
        int3 chunk_positionv = get_chunk_positionv(chunkPosition->value, chunk_dimensions);
        int3 positionv = int3_add(positionv, chunk_positionv);
        // spawn node entity here!
        const byte block_index = node->value - 1;
        if (block_index >= data->blocks_length) {
            zox_log_error("voxel [%i] is out of range [%i]", block_index, data->blocks_length)
            return;
        }
        const entity block = data->blocks[block_index];
        spawned_block_data spawned_data = (spawned_block_data) {
            .chunk = data->chunk,
            .node = node,
            .block_index = block_index,
            .block = block,
            .positionl = positionl,
            .positionv = positionv,
            .positionf = positionf,
            .scale = data->chunk_scalev,
            //.terrain_scalev = terrain_scalev,
            .render_disabled = data->render_disabled,
            .render_depth = data->render_depth,
        };
        run_hook_spawned_block(world, &spawned_data);
    }
}


void spawn_vodes(
    ecs *world,
    const entity e,
    const entity terrain,
    const byte render_depth,
    const RenderDisabled *renderDisabled,
    VoxelNode *chunk,
    const byte max_depth,
    float3 positionf,
    const float chunk_scalev,
    const float terrain_scalev
) {
    const entity realm = zox_get_value(terrain, RealmLink)
    zox_geter(realm, VoxelLinks, blocks)
    const byte blocks_length = blocks->length;
    if (blocks_length == 0) {
        return;
    }
    entity blocksarr[blocks_length];
    entity models[blocks_length];
    entity block_prefabs[blocks_length];
    byte block_vox_offsets[blocks_length];
    zero_memory(models, blocks_length, entity)
    zero_memory(block_prefabs, blocks_length, entity)
    zero_memory(block_vox_offsets, blocks_length, byte)
    for (int j = 0; j < blocks_length; j++) {
        const entity block = blocks->value[j];
        if (!zox_valid(block)) {
            continue;
        }
        blocksarr[j] = block;
        if (zox_gett_value(block, BlockModel) == zox_block_vox) {
            models[j] = zox_get_value(block, ModelLink)
            if (zox_has(block, BlockVoxOffset)) {
                block_vox_offsets[j] = zox_get_value(block, BlockVoxOffset)
            }
        }
        if (zox_has(block, BlockPrefabLink)) {
            block_prefabs[j] = zox_get_value(block, BlockPrefabLink)
        }
    }

    // why we do this?
    positionf = float3_add(positionf, float3_single(terrain_scalev));

    UpdateBlockEntities data = {
        .chunk_scalev = chunk_scalev,
        .terrain_scalev = terrain_scalev,
        .chunk = e,
        .blocks_length = blocks_length,
        .blocks = blocksarr, // metas
        .block_prefabs = block_prefabs,
        .models = models,
        .block_vox_offsets = block_vox_offsets,
        .chunk_positionf = positionf,
        .render_depth = render_depth,
        .render_disabled = renderDisabled->value,
    };
    NodeDelveData delve_data = {
        .chunk = chunk,
        .depth = 0,
        .max_depth = max_depth
    };
    spawn_vodes_dive(world, &data, &delve_data);
}

// Triggers: [VoxelNodeDirty] + [RenderDistanceDirty]
void VodesSpawnSystem(iter *it) {
    zox_sys_world()
    zox_sys_begin()
    zox_sys_in(VoxelNodeDirty)
    zox_sys_in(RenderDistanceDirty)
    // zox_sys_in(ChunkPosition)
    zox_sys_in(VoxLink)
    zox_sys_in(NodeDepth)
    zox_sys_in(RenderDisabled)
    zox_sys_in(RenderDepth)
    zox_sys_in(RenderDistance)
    zox_sys_in(Position3D)
    zox_sys_in(BlockScale)
    zox_sys_out(VoxelNode)
    zox_sys_out(BlocksSpawned)
    for (int i = 0; i < it->count; i++) {
        zox_sys_e()
        zox_sys_i(VoxelNodeDirty, voxelNodeDirty)
        zox_sys_i(RenderDistanceDirty, renderDistanceDirty)
        // zox_sys_i(ChunkPosition, chunkPosition)
        zox_sys_i(VoxLink, voxLink)
        zox_sys_i(NodeDepth, nodeDepth)
        zox_sys_i(RenderDisabled, renderDisabled)
        zox_sys_i(RenderDepth, renderDepth)
        zox_sys_i(RenderDistance, renderDistance)
        zox_sys_i(Position3D, position)
        zox_sys_i(BlockScale, scale)
        zox_sys_o(VoxelNode, node)
        zox_sys_o(BlocksSpawned, blocksSpawned)
        // either voxel node is dirty, or we are spawning for first time based on distance changes
        byte is_first_time = voxelNodeDirty->value == zox_dirty_active;
        byte is_dirty = (!blocksSpawned->value && renderDistanceDirty->value == zox_dirty_active);
        if (!is_first_time && !is_dirty) {
            continue;
        }
        //  base off render distance
        zox_geter_value(voxLink->value, NodeDepth, byte, terrain_depth);
        zox_geter_value(voxLink->value, BlockScale, float, terrain_scalev);
        byte can_spawn_vodes = renderDepth->value == terrain_depth;
        if (!can_spawn_vodes) { // renderDepth->value > block_vox_render_at_lod) { // >
            continue;
        }
        // zox_log("+ spawning block voxes at [%ix%ix%i]", chunkPosition->value.x, chunkPosition->value.y, chunkPosition->value.z)
        const byte render_depth = camera_distance_to_block_vox_depth(renderDistance->value);
        // zox_log("Spawning block voxes with render depth: %i", render_depth);

        write_lock_VoxelNode(node);
        spawn_vodes(world,
            e,
            voxLink->value,
            render_depth,
            renderDisabled,
            node,
            nodeDepth->value,
            position->value,
            scale->value,
            terrain_scalev);
        write_unlock_VoxelNode(node);
        blocksSpawned->value = 1;
    }
} zoxd_system(VodesSpawnSystem)
