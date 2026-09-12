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
    entity prefab = zox_getv(data->block, BlockPrefabLink);
    entity vox = zox_has(data->block, ModelLink) ? zox_getv(data->block, ModelLink) : 0;
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
    zox_set_parent(world, e2, data->chunk);
    // spawn_line3(world, spawn_data.positionf, float3_add(spawn_data.positionf, (float3) { 0, 2, 0 }), 2, 3);
}

typedef struct {
    byte render_disabled;
    byte render_depth;
    float3 chunk_positionf;
    float chunk_scalev;
    float terrain_block_scale;
} UpdateBlockEntities;

void spawn_vodes_dive(ecs *world,
    const UpdateBlockEntities *data,
    entity e,
    byte blocks_length,
    const byte* is_vode,
    const entity* blocks,
    VoxelNode* voxel_octree,
    byte3 position,
    byte depth,
    byte max_depth
) {
    if (!voxel_octree) {
        return;
    }
    // Dig more
    if (depth != max_depth) {
        depth++;
        position = byte3_mul1(position, 2);
        // int3_multiply_int_p(&position, 2);
        if (has_children_VoxelNode(voxel_octree)) {
            VoxelNode* kids = (VoxelNode*) voxel_octree->ptr;
            for (byte i = 0; i < octree_length; i++) {
                byte3 child_position = byte3_add(position, octree_positions[i]);
                spawn_vodes_dive(
                    world,
                    data,
                    e,
                    blocks_length,
                    is_vode,
                    blocks,
                    &kids[i],
                    child_position,
                    depth,
                    max_depth);
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
    if (block_index >= blocks_length) {
        zox_loge("Vode Block ID OOB [%i of %i]", block_index, blocks_length);
        return;
    }
    if (!is_vode[block_index]) {
        return;
    }
    // Remove and return if not a World Block
    /*entity block_prefab = block_prefabs[block_index];
    if (!block_prefab) {
        return;
    }*/
    /*if (!zox_has(block, BlockPrefabLink)) {
        return;
    }*/
    // + spawn block vox
    // if exists already, shouldn't we check if is the same block vox type?
    // if exists, and is same type, return!
    // read lock here?
    // NOTE: If air or already spawned, return
    if (!voxel_octree->value || is_linked_VoxelNode(voxel_octree)) {
        return;
    }
    int3 chunk_position = zox_getv(e, ChunkPosition);
    byte voxel_octree_depth = zox_getv(e, NodeDepth);

    float mesh_scale = data->chunk_scalev;
    // TODO: If loaded vox model, we need to scale based on the mesh we are using
    float3 positionf = byte3_to_float3(position);
    float3_scale_p(&positionf, data->terrain_block_scale);
    positionf = float3_add(positionf, data->chunk_positionf);
    positionf = float3_add(positionf, float3_single(-mesh_scale * 0.5f));
    short chunk_length = octree_size(voxel_octree_depth);
    int3 chunk_dimensions = int3_single(chunk_length);
    int3 cpositionv = get_chunk_block_position(chunk_position, chunk_dimensions);
    int3 positionv = int3_add(byte3_to_int3(position), cpositionv);
    // spawn voxel_octree entity here!
    // byte block_index = voxel_octree->value - 1;
    /*if (block_index >= blocks_length) {
        zox_log_error("voxel [%i] is out of range [%i]", block_index, blocks_length)
        return;
    }*/
    entity block = blocks[block_index];
    spawned_block_data spawned_data = (spawned_block_data) {
        .octree = voxel_octree,
        .chunk = e,
        .block_index = block_index,
        .block = block,
        .positionl = position,
        .positionv = positionv,
        .positionf = positionf,
        .scale = mesh_scale,
        .render_disabled = data->render_disabled,
        .render_depth = data->render_depth,
    };
    spawned_block_vox(world, &spawned_data);
    run_hook_spawned_block(world, &spawned_data);
}

// TODO: Break this up into two systems:
//      - Triggered by VoxelNodePostDirty or ChunkLodDirty
// Triggers: [VoxelNodeDirty] + [RenderDistanceDirty]
zox_sys2(VodesSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeDepth);
    zox_sys_in(RenderDisabled);
    zox_sys_in(RenderDepth);
    zox_sys_in(RenderDistance);
    zox_sys_in(Position3D);
    zox_sys_out(VoxelNode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(RenderDisabled, render_disabled);
        zox_sys_i(RenderDepth, render_depth);
        zox_sys_i(RenderDistance, render_distance);
        zox_sys_i(Position3D, position);
        zox_sys_o(VoxelNode, voxel_octree);
        byte is_dirty = zox_has(e, VoxelNodePostDirty);
        byte is_lod_dirty = zox_has(e, ChunkLodDirty) &&
            zox_getv(e, ChunkLodDirty);
        if (!(is_dirty || is_lod_dirty)) {
            continue;
        }
        // either voxel voxel_octree is dirty, or we are spawning for first time based on distance changes
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            continue;
        }
        entity realm = zox_get_parent(world, terrain);
        if (!zox_valid(realm)) {
            continue;
        }
        //  base off render distance
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        float terrain_scale = zox_getv(terrain, BlockScale);
        byte can_spawn_vodes = render_depth->value == terrain_depth;
        if (!can_spawn_vodes) {
            continue;
        }
        byte block_depth = camera_distance_to_block_depth(render_distance->value);
        // write_lock_VoxelNode(voxel_octree);
        // TODO: Cache these
        zox_geter(realm, BlockLinks, blocks);
        if (!blocks->length) {
            continue;
        }
        byte is_vode[blocks->length];
        zero_memory(is_vode, blocks->length, byte);
        for (ushort j = 0; j < blocks->length; j++) {
            entity block = blocks->value[j];
            if (!zox_valid(block)) {
                continue;
            }
            is_vode[j] = zox_has(block, BlockPrefabLink) &&
                zox_valid(zox_getv(block, BlockPrefabLink));
        }
        float block_scale = get_chunk_scale(depth->value, terrain_depth, terrain_scale);
        // why we do this?
        float3 positionf = float3_add(position->value, float3_single(terrain_scale));
        UpdateBlockEntities data = {
            .chunk_scalev = block_scale,
            .terrain_block_scale = terrain_scale,
            .chunk_positionf = positionf,
            .render_depth = block_depth,
            .render_disabled = render_disabled->value,
        };
        spawn_vodes_dive(
            world,
            &data,
            e,
            blocks->length,
            is_vode,
            blocks->value,
            voxel_octree,
            byte3_zero,
            0,
            depth->value);
        if (!zox_has(e, BlocksSpawned)) {
            zox_add(e, BlocksSpawned);
        }
    }
} zox_sys_end(VodesSpawnSystem);
