// NOTE: Detects intersections with voxel chunks
zox_sys2(TerrainIntersectSystem) {
    // TODO: First just make a list of voxels intersecting with
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TerrainLink);
    zox_sys_in(Rotation3D);
    zox_sys_in(Bounds3D);
    zox_sys_out(Position3D);
    zox_sys_out(Velocity3D);
    zox_sys_out(Grounded);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TerrainLink, terrain);
        zox_sys_i(Rotation3D, rotation);
        zox_sys_i(Bounds3D, bounds);
        zox_sys_o(Position3D, position);
        zox_sys_o(Velocity3D, velocity);
        zox_sys_o(Grounded, grounded);
        if (!zox_valid(terrain->value) || !zox_has(terrain->value, ChunkLinks) || !zox_has(terrain->value, BlockScale)) {
            zox_logw("Terrain Invalid");
            continue; // these shouldn't be here
        }
        // find realm first
        entity realm = zox_getv(terrain->value, RealmLink);
        if (!zox_valid(realm) || !zox_has(realm, BlockLinks)) {
            continue;
        }
        const BlockLinks* blocks = zox_get(realm, BlockLinks);
        byte terrain_depth = zox_getv(terrain->value, NodeDepth);
        float terrain_block_scale = zox_getv(terrain->value, BlockScale);
        // float3 block_size = float3_single(terrain_block_scale * 0.5f);
        zox_geter(terrain->value, ChunkLinks, chunks);
        // For each block position in bounds, check if collides with chunk
        float3 bounds_rotated = float4_rotate_bounds(rotation->value, bounds->value);
        float3 lower_bounds = float3_subtract(position->value, bounds_rotated);
        float3 upper_bounds = float3_add(position->value, bounds_rotated);
        // int3 lower_bounds_block_position = real_position_to_block_position(lower_bounds, terrain_block_scale);
        // int3 upper_bounds_block_position = real_position_to_block_position(upper_bounds, terrain_block_scale);
        float3 correction = float3_zero;
        // we just need add + terrain_block_scale and final one too
        byte hit_ground = 0;
        byte hit_axis_x = 0;
        byte hit_axis_y = 0;
        byte hit_axis_z = 0;
        int3 last_block_position;
        // NOTE: From Lower to Upper bounds, but including Upper Bounds
        float3 point;
        float add = terrain_block_scale;
        for (point.x = lower_bounds.x; point.x <= upper_bounds.x; point.x = point.x == upper_bounds.x ? upper_bounds.x + 1 : clampf(point.x + add, lower_bounds.x, upper_bounds.x)) {
            for (point.y = lower_bounds.y; point.y <= upper_bounds.y; point.y = point.y == upper_bounds.y ? upper_bounds.y + 1 : clampf(point.y + add, lower_bounds.y, upper_bounds.y)) {
                for (point.z = lower_bounds.z; point.z <= upper_bounds.z; point.z = point.z == upper_bounds.z ? upper_bounds.z + 1 : clampf(point.z + add, lower_bounds.z, upper_bounds.z)) {
                    int3 block_position = real_position_to_block_position(point, terrain_block_scale);
                    if (int3_equals(block_position, last_block_position)) {
                        continue;
                    }
                    last_block_position = block_position;
                    float3 block_positionf = block_position_to_real_position(block_position, terrain_block_scale);
                    block_positionf = float3_add(block_positionf, float3_single(terrain_block_scale / 2));
                    // Now check if intersected is solid
                    // NOTE: Assume for now
                    int3 chunk_position = block_position_to_chunk_position(block_position, terrain_depth);
                    entity chunk = int3_hashmap_get(chunks->value, chunk_position);
                    if (!zox_valid(chunk)) {
                        continue;
                    }
                    const VoxelNode* voxel_octree = zox_get(chunk, VoxelNode);
                    byte chunk_depth = zox_getv(chunk, NodeDepth);
                    byte3 local_position = block_position_to_local_position(block_position, terrain_depth, chunk_depth);
                    byte voxel = getv_VoxelNode(voxel_octree, local_position, chunk_depth);
                    byte solid = voxel ? zox_getv(blocks->value[voxel - 1], BlockCollider) != zox_block_air : 0;
                    if (!solid) {
                        continue;
                    }
                    // Get Block Face Centre
                    // NOTE: Get the closest voxel face out of 3 potential axis ones
                    float3 block_position_face_x = block_positionf;
                    if (position->value.x >= block_positionf.x) {
                        block_position_face_x.x += terrain_block_scale / 2;
                    } else {
                        block_position_face_x.x -= terrain_block_scale / 2;
                    }
                    float3 block_position_face_y = block_positionf;
                    if (position->value.y >= block_positionf.y) {
                        block_position_face_y.y += terrain_block_scale / 2;
                    } else {
                        block_position_face_y.y -= terrain_block_scale / 2;
                    }
                    float3 block_position_face_z = block_positionf;
                    if (position->value.z >= block_positionf.z) {
                        block_position_face_z.z += terrain_block_scale / 2;
                    } else {
                        block_position_face_z.z -= terrain_block_scale / 2;
                    }
                    // TODO: Work out a better way to check closest face
                    // Gets closest face XYZ
                    float block_face_dist_x = float3_distance(position->value, block_position_face_x);
                    float block_face_dist_y = float3_distance(position->value, block_position_face_y);
                    float block_face_dist_z = float3_distance(position->value, block_position_face_z);
                    byte block_axis = block_face_dist_x < block_face_dist_y && block_face_dist_x < block_face_dist_z ? 0 : block_face_dist_y < block_face_dist_z ? 1 : 2;
                    float3 block_position_face = block_face_dist_x < block_face_dist_y && block_face_dist_x < block_face_dist_z ? block_position_face_x : block_face_dist_y < block_face_dist_z ? block_position_face_y : block_position_face_z;
                    // NOTE: Get the Face Direction as a byte
                    byte face_direction = 0;
                    if (block_axis == 0) {
                        if (position->value.x > block_position_face.x) {
                            face_direction = direction_right;
                        } else {
                            face_direction = direction_left;
                        }
                    } else if (block_axis == 1) {
                        if (position->value.y > block_position_face.y) {
                            face_direction = direction_up;
                        } else {
                            face_direction = direction_down;
                        }
                    } else if (block_axis == 2) {
                        if (position->value.z > block_position_face.z) {
                            face_direction = direction_front;
                        } else {
                            face_direction = direction_back;
                        }
                    }
                    // TODO: Check if direction adjacent node is solid
                    zox_geter(chunk, ChunkNeighbors, neighbors);
                    const VoxelNode* neighbor_voxel_octrees[6];
                    for (int a = 0; a < 6; a++) {
                        entity neighbor = neighbors->value[a];
                        if (!zox_valid(neighbor) || !zox_has(neighbor, VoxelNode)) {
                            neighbor_voxel_octrees[a] = NULL;
                        } else {
                            neighbor_voxel_octrees[a] = zox_get(neighbor, VoxelNode);
                        }
                    }
                    const VoxelNode* adjacent = get_neighbor_VoxelNode(voxel_octree, neighbor_voxel_octrees, face_direction, local_position, chunk_depth);
                    byte adjacent_solid = 0;
                    if (adjacent && adjacent->value) {
                        entity block = blocks->value[adjacent->value - 1];
                        if (zox_valid(block)) {
                            adjacent_solid = !zox_has(block, BlockCollider) || zox_getv(block, BlockCollider) != zox_block_air;
                        }
                    }
                    if (adjacent_solid) {
                        continue;
                    }
                    // NOTE: Calculate correction for moving away from blocks
                    float3 penetration = float3_subtract(block_position_face, point);
                    if (block_axis == 0 && float_abs(penetration.x) > float_abs(correction.x)) {
                        correction.x = penetration.x;
                    }
                    if (block_axis == 1 && float_abs(penetration.y) > float_abs(correction.y)) {
                        correction.y = penetration.y;
                    }
                    if (block_axis == 2 && float_abs(penetration.z) > float_abs(correction.z)) {
                        correction.z = penetration.z;
                    }
                    if (block_axis == 0) {
                        hit_axis_x = 1;
                    }
                    if (block_axis == 1) {
                        hit_axis_y = 1;
                    }
                    if (block_axis == 2) {
                        hit_axis_z = 1;
                    }
                    if (block_axis == 1 && position->value.y > block_position_face.y) {
                        hit_ground = 1;
                    }
                }
            }
        }
        grounded->value = hit_ground;
        if (hit_axis_x) {
            velocity->value.x *= -bounce_lost_force.x;
        }
        if (hit_axis_y) {
            velocity->value.y *= -bounce_lost_force.y;
        }
        if (hit_axis_z) {
            velocity->value.z *= -bounce_lost_force.z;
        }
        // show correction vector
        if (!float3_equals(correction, float3_zero)) {
            position->value = float3_add(position->value, correction);
        }
    }
} zox_sys_end(TerrainIntersectSystem);
