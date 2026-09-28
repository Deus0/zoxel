// Our main spawn function
void spawned_block_vox(
    ecs* world,
    VoxelNode* octree,
    entity chunk,
    byte block_index,
    entity block,
    byte3 positionl,
    int3 positionv,
    float3 positionf,
    float scale,
    byte render_disabled,
    byte render_depth)
{
    if (!zox_has(block, BlockPrefabLink)) {
        return;
    }
    if (!octree) {
        zox_loge("null octree in [spawned_block_vox]")
        return;
    }
    // gett block data
    entity prefab = zox_getv(block, BlockPrefabLink);
    entity vox =
        zox_has(block, ModelLink) ?
            zox_getv(block, ModelLink) :
            0;
    entity e2;
    if (zox_has(prefab, BlockVox)) {
        e2 = spawn_block_vox(
            world,
            prefab,
            block,
            vox,
            block_index,
            render_depth,
            render_disabled,
            positionf,
            scale);
    } else if (zox_has(prefab, RendererInstance)) {
        e2 = spawn_block_vox_instanced(
            world,
            prefab,
            block,
            vox,
            block_index,
            render_depth,
            render_disabled,
            positionl,
            positionv,
            positionf);
    } else {
        return;
    }
    if (!e2) {
        // failed spawning
        return;
    }
    link_node_VoxelNode(octree, e2);
    zox_set_parent(world, e2, chunk);
    // spawn_line3(world, spawn_data.positionf, float3_add(spawn_data.positionf, (float3) { 0, 2, 0 }), 2, 3);
}

typedef struct {
    byte render_disabled;
    byte render_depth;
    float3 chunk_positionf;
    float chunk_scalev;
    float terrain_block_scale;
} UpdateBlockEntities;

void spawn_vodes_dive(
    ecs* world,
    const UpdateBlockEntities* data,
    entity chunk,
    byte blocks_length,
    const byte* is_vode,
    const entity* blocks,
    VoxelNode* octree,
    byte3 position,
    byte depth,
    byte max_depth,
    int3 chunk_block_position
) {
    if (!octree) {
        return;
    }
    // Dig more
    if (depth != max_depth) {
        depth++;
        position = byte3_mul1(position, 2);
        // int3_multiply_int_p(&position, 2);
        if (has_children_VoxelNode(octree)) {
            VoxelNode* kids = (VoxelNode*) octree->ptr;
            for (byte i = 0; i < octree_length; i++) {
                byte3 child_position = byte3_add(position, octree_positions[i]);
                spawn_vodes_dive(
                    world,
                    data,
                    chunk,
                    blocks_length,
                    is_vode,
                    blocks,
                    &kids[i],
                    child_position,
                    depth,
                    max_depth,
                    chunk_block_position
                );
            }
        }
        return;
    }
    // air returns!
    if (!octree->value) {
        return;
    }
    // check if out of bounds
    byte block_index = octree->value - 1;
    if (block_index >= blocks_length) {
        zox_loge("Vode Block ID OOB [%i of %i]",
            block_index,
            blocks_length);
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
    if (!octree->value || is_linked_VoxelNode(octree)) {
        return;
    }
    // getters
    //int3 chunk_position = zox_getv(chunk, ChunkPosition);
    // byte octree_depth = zox_getv(chunk, NodeDepth);
    // calculations
    // TODO: If loaded vox model, we need to scale based on the mesh we are using
    float mesh_scale = data->chunk_scalev;
    float3 positionf = byte3_to_float3(position);
    positionf = float3_scale1(positionf, data->terrain_block_scale);
    positionf = float3_add(
        positionf,
        data->chunk_positionf);
    positionf = float3_add(
        positionf,
        float3_single(-data->chunk_scalev * 0.5f));
    int3 positionv = int3_add(
        byte3_to_int3(position),
        chunk_block_position);
    entity block = blocks[block_index];
    spawned_block_vox(
        world,
        octree,
        chunk,
        block_index,
        block,
        position,
        positionv,
        positionf,
        data->chunk_scalev,
        data->render_disabled,
        data->render_depth
    );
    spawned_block_data spawned_data = (spawned_block_data) {
        .octree = octree,
        .chunk = chunk,
        .block_index = block_index,
        .block = block,
        .positionl = position,
        .positionv = positionv,
        .positionf = positionf,
        .scale = data->chunk_scalev,
        .render_disabled = data->render_disabled,
        .render_depth = data->render_depth,
    };
    run_hook_spawned_block(world, &spawned_data);
}

// TODO: Break this up into two systems:
//      - Triggered by VoxelNodePostDirty or ChunkLodDirty
// Triggers: [VoxelNodeDirty] + [RenderDistanceDirty]
void vodes_spawn_system(iter* it) {
    if (zox_disable_vodes) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodeDepth);
    zox_sys_in(RenderDisabled);
    zox_sys_in(RenderDistance);
    zox_sys_in(Position3D);
    zox_sys_in(ChunkPosition);
    zox_sys_out(VoxelNode);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodeDepth, depth);
        zox_sys_i(RenderDisabled, render_disabled);
        zox_sys_i(RenderDistance, render_distance);
        zox_sys_i(Position3D, position);
        zox_sys_i(ChunkPosition, chunk_position);
        zox_sys_o(VoxelNode, octree);
        byte is_dirty = zox_has(e, VoxelNodePostDirty);
        //byte is_lod_dirty = zox_has(e, ChunkLodDirty) &&
        //    zox_getv(e, ChunkLodDirty);
        byte has_vodes = zox_has(e, BlocksSpawned);
        if (!(is_dirty || !has_vodes)) {
            continue;
        }
        // either voxel octree is dirty, or we are spawning for first time based on distance changes
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
        /*byte can_spawn_vodes = render_depth->value == terrain_depth;
        if (!can_spawn_vodes) {
            continue;
        }*/
        byte block_depth = camera_distance_to_block_depth(render_distance->value);
        // write_lock_VoxelNode(octree);
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
        float block_scale = get_chunk_scale(
            depth->value,
            terrain_depth,
            terrain_scale);
        // why we do this?
        float3 positionf = float3_add(
            position->value,
            float3_single(terrain_scale));

        ushort chunk_length = octree_size(depth->value);
        int3 chunk_block_position = get_chunk_block_position(
            chunk_position->value,
            int3_single(chunk_length));

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
            octree,
            byte3_zero,
            0,
            depth->value,
            chunk_block_position);
        if (!has_vodes) {
            zox_add(e, BlocksSpawned);
        }
    }
} zoxd_system(vodes_spawn_system);
