// if character falls through map, push up
// todo: draw lines on this to test why it suddenly detects..!
byte unstuck_log_count = 0;

#ifdef zox_dbg_lines_unstuck
extern entity spawn_line3D(ecs*, float3, float3, float, double);
#endif

// TODO: Dont cache a position, just raycast in direction of gravity
// TODO: Spawn debug lines when repositioning
zox_sys2(UnstuckSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TerrainLink);
    zox_sys_in(Bounds3D);
    zox_sys_out(LastUnstuck3);
    zox_sys_out(InsideBlock);
    zox_sys_out(InsideBlockDirty);
    zox_sys_out(Position3D);

    // cache voxels and colliders for speed
    const BlockLinks *voxels = get_first_terrain_voxels(world, TerrainLink_, it->count);
    if (!voxels) {
        zox_log_error("UnstuckSystem: No BlockLinks");
        return;
    }

    byte colliders[voxels->length + 1];
    get_block_collisions(world, voxels, colliders);

    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TerrainLink, link);
        zox_sys_i(Bounds3D, bounds);
        zox_sys_o(LastUnstuck3, last);
        zox_sys_o(InsideBlock, inside);
        zox_sys_o(InsideBlockDirty, idirty);
        zox_sys_o(Position3D, position);

        zox_geter(link->value, ChunkLinks, chunks);
        zox_geter_value(link->value, BlockScale, float, terrain_scale);
        zox_geter_value(link->value, NodeDepth, byte, terrain_depth);
        float3 unstuck_push = (float3) { 0, terrain_scale, 0 };

        float3 pointf = float3_add(position->value, (float3) { 0, bounds->value.y / 2.0f, 0 });
        int3 pointv = positionf_to_positionv(pointf, terrain_scale);
        byte3 max_chunk_size = byte3_single(powers_of_two[terrain_depth]);
        int3 pointc = positionv_to_positionc(pointv, max_chunk_size);

        float3 lastf = float3_add(last->value, (float3) { 0, bounds->value.y / 2.0f, 0 });
        int3 lastv = positionf_to_positionv(lastf, terrain_scale);

        entity chunk = int3_hashmap_get(chunks->value, pointc);
        if (!zox_valid(chunk)) {
            // zox_sys_e();
            // zox_log_error("[%s] Chunk Not Found [%lu] v[%ix%ix%i] c[%ix%ix%i]", zox_get_name(e), chunk, pointv.x, pointv.y, pointv.z, pointc.x, pointc.y, pointc.z);
            if (position->value.y < 0) {
                if (!float3_equals(last->value, float3_zero)) {
                    position->value = float3_add(last->value, unstuck_push);
                    // zox_log_error("[%s] Chunk Not Found [%lu] v[%ix%ix%i] c[%ix%ix%i]", zox_get_name(e), chunk, pointv.x, pointv.y, pointv.z, pointc.x, pointc.y, pointc.z);
                }
            }
            continue;
        }

        if (!zox_has(chunk, VoxelNode)) {
            zox_log_error("chunk [%s:%lu] doesn't have a VoxelNode!", zox_get_name(chunk), chunk);
            continue;
        }

        zox_geter(chunk, VoxelNode, node);
        zox_geter_value(chunk, NodeDepth, byte, cdepth);

        // const byte3 chunk_size = byte3_single(powers_of_two[node_depth]);
        // positions
        //const int3 pointv = positionf_to_positionv(pointf, terrain_scale);
        //byte3 pointl = get_positionl_byte3(pointv, chunk_size);
        byte3 chunk_size = byte3_single(powers_of_two[cdepth]);
        byte3 pointl = get_positionl_byte3(pointv, chunk_size);

        if (!byte3_in_bounds(pointl, chunk_size)) {
            zox_log_error("Voxel Local OOB: [%ix%ix%i] :: %i", pointl.x, pointl.y, pointl.z, powers_of_two[cdepth]);
            continue;
        }

        // voxel
        byte voxel = get_sub_node_voxel_locked(node, &pointl, cdepth);

        if (inside->value != voxel) {
            inside->value = voxel;
            idirty->value = zox_dirty_trigger;
            // zox_log("Inside new block [%i]", voxel);
        } else if (!int3_equals(pointv, lastv)) {
            // Compares voxel position and triggers when moving into same block type
            idirty->value = zox_dirty_trigger;
        }

        // If in air, we just cache inside position
        if (!voxel || !colliders[voxel]) {
            last->value = position->value;
            continue;
        }

        // If Hasn't been unstuck
        if (float3_equals(last->value, float3_zero)) {
            last->value = float3_add(position->value, unstuck_push);
            /*position->value = float3_add(position->value, unstuck_push);
            if (unstuck_log_count < 10) {
                zox_logw("Character never unstuck v[%ix%ix%i] l[%ix%ix%i]", pointv.x, pointv.y, pointv.z, pointl.x, pointl.y,  pointl.z);
                unstuck_log_count++;
            }*/
            continue;
        }

        // float3 reverse_point = float3_subtract(last->value, (float3) { 0, bounds->value.y / 2.0f, 0 });
#ifdef zox_dbg_lines_unstuck
        float3 ogposition = position->value;
#endif

        position->value = float3_add(last->value, unstuck_push);

#ifdef zox_dbg_lines_unstuck
        spawn_line3D(world, ogposition, position->value, 0.5f, 0.1);
#endif

        // zox_log_error("Character Unstuck v[%ix%ix%i] l[%ix%ix%i] to f[%f.1x%f.1x%f.1]", pointv.x, pointv.y, pointv.z, pointl.x, pointl.y,  pointl.z, last->value.x, last->value.y, last->value.z);

    }
} zox_sys_end(UnstuckSystem);
