// if character falls through map, push up
// todo: draw lines on this to test why it suddenly detects..!
byte unstuck_log_count = 0;
#ifdef zox_dbg_lines_unstuck
extern entity spawn_line3c(ecs*, float3, float3, float, double, color_rgb);
#endif
// TODO: Dont cache a position, just raycast in direction of gravity
// TODO: Spawn debug lines when repositioning
zox_sys2(UnstuckSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TerrainLink);
    zox_sys_in(Bounds3D);
    zox_sys_out(LastUnstuck3);
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
        zox_sys_o(Position3D, position);
        zox_geter(link->value, ChunkLinks, chunks);
        zox_geter_value(link->value, BlockScale, float, terrain_scale);
        zox_geter_value(link->value, NodeDepth, byte, terrain_depth);
        // float3 unstuck_push = (float3) { 0, terrain_scale, 0 };
        float3 poffset = (float3) { 0, - bounds->value.y / 2.0f, 0 };
        float3 pointf = float3_add(position->value, poffset);
        int3 pointv = real_position_to_block_position(pointf, terrain_scale);
        byte3 max_chunk_size = byte3_single(powers_of_two[terrain_depth]);
        int3 pointc = block_position_to_positionc(pointv, max_chunk_size);
        // float3 lastf = float3_add(last->value, (float3) { 0, bounds->value.y / 2.0f, 0 });
        // int3 lastv = real_position_to_block_position(lastf, terrain_scale);
        entity chunk = int3_hashmap_get(chunks->value, pointc);
        if (!zox_valid(chunk)) {
            // zox_sys_e();
            // zox_log_error("[%s] Chunk Not Found [%lu] v[%ix%ix%i] c[%ix%ix%i]", zox_get_name(e), chunk, pointv.x, pointv.y, pointv.z, pointc.x, pointc.y, pointc.z);
            if (position->value.y < 0) {
                if (!float3_equals(last->value, float3_zero)) {
                    position->value.y = 100;
                    zox_sys_e();
                    zox_logw("Character [%s] Fell through map", zox_get_name(e));
                    // position->value = float3_add(last->value, unstuck_push);
                    // zox_log_error("[%s] Chunk Not Found [%lu] v[%ix%ix%i] c[%ix%ix%i]", zox_get_name(e), chunk, pointv.x, pointv.y, pointv.z, pointc.x, pointc.y, pointc.z);
                }
            }
            continue;
        }
        if (!zox_has(chunk, VoxelNode)) {
            zox_log_error("chunk [%s:%lu] doesn't have a VoxelNode!", zox_get_name(chunk), chunk);
            continue;
        }
        zox_geter(chunk, VoxelNode, voctree);
        zox_geter_value(chunk, NodeDepth, byte, cdepth);
        // const byte3 chunk_size = byte3_single(powers_of_two[node_depth]);
        // positions
        //const int3 pointv = real_position_to_block_position(pointf, terrain_scale);
        //byte3 pointl = get_positionl_byte3(pointv, chunk_size);
        byte3 chunk_size = byte3_single(powers_of_two[cdepth]);
        byte3 pointl = get_positionl_byte3(pointv, chunk_size);
        if (!byte3_in_bounds(pointl, chunk_size)) {
            zox_log_error("Voxel Local OOB: [%ix%ix%i] :: %i", pointl.x, pointl.y, pointl.z, powers_of_two[cdepth]);
            continue;
        }
        // voxel
        byte voxel = get_value_VoxelNode(voctree, cdepth, pointl, 0);
        // byte voxel = get_sub_node_voxel_locked(voctree, &pointl, cdepth);
        /*if (inside->value != voxel) {
            inside->value = voxel;
            idirty->value = zox_dirty_trigger;
            // zox_log("Inside new block [%i]", voxel);
        } else if (!int3_equals(pointv, lastv)) {
            // Compares voxel position and triggers when moving into same block type
            idirty->value = zox_dirty_trigger;
        }*/
        // If in air, we just cache inside position
        if (!voxel || !colliders[voxel]) {
            last->value = position->value;
            continue;
        }
        // If Hasn't been unstuck
        if (float3_equals(last->value, float3_zero)) {
            // last->value = float3_add(position->value, unstuck_push);
            // position->value = last->value;
            continue;
        }
        // float3 reverse_point = float3_subtract(last->value, (float3) { 0, bounds->value.y / 2.0f, 0 });
#ifdef zox_dbg_lines_unstuck
        float3 ogposition = position->value;
#endif
        // position->value = float3_add(last->value, unstuck_push);
        // Set new position is above ground
        int positionv = real_position_to_block_position1(position->value.y + poffset.y, terrain_scale);
        // positionv.y ++; // move up / - gravity direction
        float distance_to_above_ground = block_position_to_real_position1(positionv + 1, terrain_scale) - (position->value.y + poffset.y);
        // position->value.y = block_position_to_real_position1(positionv, terrain_scale).y;
        float buffer_up = 0.01f;
        position->value.y += distance_to_above_ground + buffer_up;
        // reverse vel down
        zox_sys_e();
        if (zox_has(e, Velocity3D)) {
            zox_geter_value(e, Velocity3D, float3, vel);
            position->value.y -= vel.y;
        }
#ifdef zox_dbg_lines_unstuck
        float dthickness = 2.5f;
        float dtime = 15;
        spawn_line3c(world, ogposition, float3_add(ogposition, (float3) { 0, distance_to_above_ground, 0 }), dthickness, dtime, color_rgb_red);
        spawn_line3c(world, ogposition, float3_add(ogposition, (float3) { 0, 0, terrain_scale }), dthickness, dtime, color_rgb_cyan);
        // spawn_line3c(world, ogposition, position->value, dthickness, dtime, color_rgb_red);
        // spawn_line3c(world, ogposition, float3_add(ogposition, float3_forward), dthickness, dtime, color_rgb_green);
        // spawn_line3c(world, pointf, float3_add(pointf, float3_right), dthickness, dtime, color_rgb_cyan);
#endif
        // zox_log_error("Character Unstuck v[%ix%ix%i] l[%ix%ix%i] to f[%f.1x%f.1x%f.1]", pointv.x, pointv.y, pointv.z, pointl.x, pointl.y,  pointl.z, last->value.x, last->value.y, last->value.z);
    }
} zox_sys_end(UnstuckSystem);
