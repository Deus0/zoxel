/*
AABB entity
AABB voxel
    |
    + smallest overlap axis
        |
        + collision normal
*/

// NOTE: Detects intersections with voxel chunks
void terrain_collision_system(iter* it) {
    // TODO: First just make a list of voxels intersecting with
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Rotation3D);
    zox_sys_in(Bounds3D);
    zox_sys_out(Position3D);
    zox_sys_out(Velocity3D);
    zox_sys_out(Grounded);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Rotation3D, rotation);
        zox_sys_i(Bounds3D, bounds);
        zox_sys_o(Position3D, position);
        zox_sys_o(Velocity3D, velocity);
        zox_sys_o(Grounded, grounded);
        entity terrain = zox_get_link(world, e, TerrainLink);
        if (!zox_valid(terrain) || !zox_has(terrain, ChunkLinks) || !zox_has(terrain, BlockScale)) {
            zox_logw("Terrain Invalid");
            continue; // these shouldn't be here
        }
        // find realm first
        entity realm = zox_get_link(world, terrain, RealmLink);
        if (!zox_valid(realm) || !zox_has(realm, BlockLinks)) {
            zox_loge("Blocks Manager missing Terrain [%s]",
                zox_getn(terrain));
            continue;
        }
        const BlockLinks* blocks = zox_get(realm, BlockLinks);
        byte terrain_depth = zox_getv(terrain, NodeDepth);
        float terrain_block_scale = zox_getv(terrain, BlockScale);
        zox_geter(terrain, ChunkLinks, chunks);
        // For each block position in bounds, check if collides with chunk
        float3 bounds_rotated = float4_rotate_bounds(rotation->value, bounds->value);
        float3 lower_bounds = float3_subtract(position->value, bounds_rotated);
        float3 upper_bounds = float3_add(position->value, bounds_rotated);
        float3 correction = float3_zero;
        // we just need add + terrain_block_scale and final one too
        byte hit_ground = 0;
        byte hit_axis_x = 0;
        byte hit_axis_y = 0;
        byte hit_axis_z = 0;
        int3 last_block_position = (int3) { 0, -10000, 0 };
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
                    byte voxel = getv_VoxelNode(voxel_octree, chunk_depth, local_position);
                    byte solid = voxel ?
                        zox_getv(blocks->value[voxel - 1], BlockCollider) != zox_block_air :
                        0;
                    if (!solid) {
                        continue;
                    }

                    // Face Selection
                    float half_block = terrain_block_scale * 0.5f;

                    // entity extents
                    float3 entity_half = bounds_rotated;

                    // voxel extents
                    float3 voxel_half = {
                        half_block,
                        half_block,
                        half_block
                    };

                    float3 delta = float3_subtract(position->value, block_positionf);

                    // overlap amount on each axis
                    float overlap_x = (entity_half.x + voxel_half.x) - float_abs(delta.x);
                    float overlap_y = (entity_half.y + voxel_half.y) - float_abs(delta.y);
                    float overlap_z = (entity_half.z + voxel_half.z) - float_abs(delta.z);

                    // ignore if not actually intersecting
                    if (overlap_x <= 0 || overlap_y <= 0 || overlap_z <= 0) {
                        continue;
                    }

                    // Which axis did we collide with
                    byte block_axis;
                    // which face we hit
                    byte face_direction;
                    float3 penetration = float3_zero;

                    // smallest penetration axis
                    if (overlap_x <= overlap_y && overlap_x <= overlap_z) {
                        block_axis = 0;
                        penetration.x = delta.x > 0 ? overlap_x : -overlap_x;
                        face_direction = delta.x > 0 ? direction_right : direction_left;
                    } else if (overlap_y <= overlap_z) {
                        block_axis = 1;
                        penetration.y = delta.y > 0 ? overlap_y : -overlap_y;
                        face_direction = delta.y > 0 ? direction_up : direction_down;
                    } else {
                        block_axis = 2;
                        penetration.z = delta.z > 0 ? overlap_z : -overlap_z;
                        face_direction = delta.z > 0 ? direction_front : direction_back;
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

                    // Picks the largest penetration vector
                    if (block_axis == 0 && float_abs(penetration.x) > float_abs(correction.x))
                        correction.x = penetration.x;

                    if (block_axis == 1 && float_abs(penetration.y) > float_abs(correction.y))
                        correction.y = penetration.y;

                    if (block_axis == 2 && float_abs(penetration.z) > float_abs(correction.z))
                        correction.z = penetration.z;


                    if (block_axis == 0) {
                        hit_axis_x = 1;
                    }
                    if (block_axis == 1) {
                        hit_axis_y = 1;
                    }
                    if (block_axis == 2) {
                        hit_axis_z = 1;
                    }
                    if (block_axis == 1 && correction.y > 0) { //  position->value.y > block_position_face.y) {
                        hit_ground = 1;
                    }
                }
            }
        }
        if (!hit_ground) {
            if (grounded->value == ground_state_grounded) {
                grounded->value = ground_state_leaving;
            } else if (grounded->value == ground_state_leaving) {
                grounded->value = ground_state_air;
            }
        } else {
            grounded->value = ground_state_grounded;
        }
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
} zoxd_system(terrain_collision_system);
