// TODO: Remove Struct use
// TODO: Make use Queue Systems for updates on Vodes

/*typedef struct {
    VoxelNode *chunk;
    int3 octree_position;
    byte depth;
    byte max_depth;
} NodeDelveData;*/

typedef struct {
    entity chunk;
    const entity *blocks;
    const entity *models;
    const entity *block_prefabs;
    const byte *block_vox_offsets;
    byte blocks_length;
    float3 chunk_positionf;
    float chunk_scalev;
    float terrain_block_scale;
    byte render_disabled;
    byte render_depth;
} UpdateBlockEntities;

// Our main spawn function
void spawned_block_vox(ecs *world, spawned_block_data* data) {
    if (!zox_has(data->block, BlockPrefabLink)) {
        return;
    }
    if (!data->octree) {
        zox_log_error("null voxel_octree in [spawned_block_vox]")
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
    link_node_VoxelNode(data->octree, e2);
    // spawn_line3(world, spawn_data.positionf, float3_add(spawn_data.positionf, (float3) { 0, 2, 0 }), 2, 3);
}

/*zox_log("+ Placing Block [%s]: linked: [%i]", zox_get_name(data->block), data->voxel_octree->linked)
zox_log("   - local [%ix%ix%i] ", spawn_data.positionl.x, spawn_data.positionl.y, spawn_data.positionl.z)
zox_log("   - global [%ix%ix%i] ", spawn_data.positionv.x, spawn_data.positionv.y, spawn_data.positionv.z)
zox_log("   - real [%fx%fx%f] ", spawn_data.positionf.x, spawn_data.positionf.y, spawn_data.positionf.z)*/

void spawn_vodes_dive(ecs *world, const UpdateBlockEntities *data, VoxelNode* voxel_octree, int3 position, byte depth, byte max_depth) {
    // NodeDelveData *delve_data) {
    // VoxelNode *voxel_octree = delve_data->chunk;
    if (!voxel_octree) {
        return;
    }
    // Dig more
    if (depth != max_depth) {
        depth++;
        int3_multiply_int_p(&position, 2);
        if (has_children_VoxelNode(voxel_octree)) {
            VoxelNode* kids = get_children_VoxelNode(voxel_octree);
            for (byte i = 0; i < octree_length; i++) {
                int3 child_position = int3_add(position, octree_positions[i]);
                spawn_vodes_dive(world, data, &kids[i], child_position, depth, max_depth);
            }
        }
        return;
    }
    // air returns!
    if (!voxel_octree->value) {
        return;
    }
    // check if out of bounds
    byte block_index = voxel_octree->value - 1;
    if (block_index >= data->blocks_length) {
        zox_loge("Vode Block ID OOB [%i of %i]", block_index, data->blocks_length);
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
    if (is_linked_VoxelNode(voxel_octree)) {
        return;
    }
    float mesh_scale = data->chunk_scalev;
    // TODO: If loaded vox model, we need to scale based on the mesh we are using
    byte3 positionl = int3_to_byte3(position);
    int3 positionv = position;
    float3 positionf = float3_from_int3(positionv);
    float3_scale_p(&positionf, data->terrain_block_scale);
    float3_add_float3_p(&positionf, data->chunk_positionf);
    float3_add_float3_p(&positionf, float3_single(-mesh_scale * 0.5f));
    if (voxel_octree->value && !is_linked_VoxelNode(voxel_octree)) {
        zox_geter(data->chunk, ChunkPosition, cposition);
        zox_geter_value(data->chunk, NodeDepth, byte, voxel_octree_depth);
        int chunk_length = powers_of_two[voxel_octree_depth];
        int3 chunk_dimensions = int3_single(chunk_length);
        int3 cpositionv = get_chunk_block_position(cposition->value, chunk_dimensions);
        int3 positionv = int3_add(positionv, cpositionv);
        // spawn voxel_octree entity here!
        byte block_index = voxel_octree->value - 1;
        if (block_index >= data->blocks_length) {
            zox_log_error("voxel [%i] is out of range [%i]", block_index, data->blocks_length)
            return;
        }
        entity block = data->blocks[block_index];
        spawned_block_data spawned_data = (spawned_block_data) {
            .octree = voxel_octree,
            .chunk = data->chunk,
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

void spawn_vodes(ecs* world, entity e, entity terrain, byte render_depth, byte render_disabled, VoxelNode* octree, byte max_depth, float3 positionf, float chunk_scalev, float terrain_block_scale) {
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
    positionf = float3_add(positionf, float3_single(terrain_block_scale));
    UpdateBlockEntities data = {
        .chunk_scalev = chunk_scalev,
        .terrain_block_scale = terrain_block_scale,
        .chunk = e,
        .blocks_length = blocks_length,
        .blocks = blocksarr, // metas
        .block_prefabs = block_prefabs,
        .models = models,
        .block_vox_offsets = block_vox_offsets,
        .chunk_positionf = positionf,
        .render_depth = render_depth,
        .render_disabled = render_disabled,
    };
    spawn_vodes_dive(world, &data, octree, int3_zero, 0, max_depth); //, &delve_data);
}

// Triggers: [VoxelNodeDirty] + [RenderDistanceDirty]
zox_sys2(VodesSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNodeDirty);
    zox_sys_in(RenderDistanceDirty);
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
        zox_sys_i(VoxelNodeDirty, voxel_octree_dirty);
        zox_sys_i(RenderDistanceDirty, render_distance_dirty);
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(RenderDisabled, render_disabled);
        zox_sys_i(RenderDepth, render_depth);
        zox_sys_i(RenderDistance, render_distance);
        zox_sys_i(Position3D, position);
        zox_sys_i(BlockScale, scale);
        zox_sys_o(VoxelNode, voxel_octree);
        zox_sys_o(BlocksSpawned, spawned);
        // either voxel voxel_octree is dirty, or we are spawning for first time based on distance changes
        byte is_first_time = voxel_octree_dirty->value == zox_dirty_active;
        byte is_dirty = (!spawned->value && render_distance_dirty->value == zox_dirty_active);
        if (!is_first_time && !is_dirty) {
            continue;
        }
        entity terrain = zox_get_parent(world, e);
        //  base off render distance
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        float terrain_block_scale = zox_getv(terrain, BlockScale);
        byte can_spawn_vodes = render_depth->value == terrain_depth;
        if (!can_spawn_vodes) {
            continue;
        }
        byte rdepth = camera_distance_to_block_vox_depth(render_distance->value);
        write_lock_VoxelNode(voxel_octree);
        spawn_vodes(world, e, terrain, rdepth, render_disabled->value, voxel_octree, depth->value, position->value, scale->value, terrain_block_scale);
        write_unlock_VoxelNode(voxel_octree);
        spawned->value = 1;
    }
} zox_sys_end(VodesSpawnSystem);
