byte zox_dbg_intersect = 0;

void toggle_dbg_intersect(ecs* world, ClickEventData data) {
    zox_dbg_intersect = !zox_dbg_intersect;
    zox_log("Debugging Intersects [%s]", zox_dbg_intersect ? "Enabled" : "Disabled");
}

// NOTE:Purely show the intersections
zox_sys2(TerrainIntersectDebugSystem) {
    if (!zox_dbg_intersect) {
        return;
    }
    // NOTE: Remember to put system on mainthread if debugging
    color dbg_color_air = (color) { 0, 255, 255, 125 };
    color dbg_color_solid = (color) { 155, 25, 25, 125 };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TerrainLink);
    zox_sys_in(Position3D);
    zox_sys_in(Rotation3D);
    zox_sys_in(Bounds3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TerrainLink, terrain);
        zox_sys_i(Position3D, position);
        zox_sys_i(Rotation3D, rotation);
        zox_sys_i(Bounds3D, bounds);
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
        float3 block_size = float3_single(terrain_block_scale * 0.5f);
        zox_geter(terrain->value, ChunkLinks, chunks);
        // For each block position in bounds, check if collides with chunk
        float3 bounds_rotated = float4_rotate_bounds(rotation->value, bounds->value);
        float3 lower_bounds = float3_subtract(position->value, bounds_rotated);
        float3 upper_bounds = float3_add(position->value, bounds_rotated);
        // int3 lower_bounds_block_position = real_position_to_block_position(lower_bounds, terrain_block_scale);
        // int3 upper_bounds_block_position = real_position_to_block_position(upper_bounds, terrain_block_scale);
        float3 correction = float3_zero;
        // we just need add + terrain_block_scale and final one too
        int3 last_block_position = (int3) { 0, -10000, 0 };
        // NOTE: Show Bounds
        spawn_cube_lines_rgba(world, position->value, bounds_rotated, 4, color_white, 0.01);
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
                    color fill = solid ? dbg_color_solid : dbg_color_air;
                    // NOTE: Shows Voxel Info of Intersections
                    spawn_cube_lines_rgba(world, block_positionf, block_size, 2, fill, 0.01);
                    if (!solid) {
                        continue;
                    }
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
                    spawn_cube_lines_rgba(world, block_positionf, block_size, 2, fill, 0.01);
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
                    // NOTE: Get Adjacent voxel, if solid we skip this collision
                    // face direction
                    zox_geter(chunk, ChunkNeighbors, neighbors);
                    const VoxelNode* neighbor_voxel_octrees[6];
                    for (int a = 0; a < 6; a++) {
                        entity neighbor = neighbors->value[a];
                        if (!neighbor) {
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
                    // NOTE: Shows Block Face with adjacent direction
                    color face_color = adjacent_solid ? color_red : color_green;
                    float3 face_end_point = float3_add(block_position_face, float3_scale(direction_to_normal(face_direction), terrain_block_scale / 3));
                    spawn_cube_lines_rgba(world, block_position_face, float3_single(0.01f), 2, face_color, 0.016);
                    spawn_line3_alpha(world, block_position_face, face_end_point, 4, 0.01, face_color);
                    spawn_cube_lines_rgba(world, face_end_point, float3_single(0.01f), 2, face_color, 0.01);
                    if (adjacent_solid) {
                        continue;
                    }
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
                    /*if (block_axis == 1 && position->value.y > block_position_face.y) {
                        hit_ground = 1;
                    }
                    if (block_axis == 0) {
                        hit_axis_x = 1;
                    }
                    if (block_axis == 1) {
                        hit_axis_y = 1;
                    }
                    if (block_axis == 2) {
                        hit_axis_z = 1;
                    }*/
                    // NOTE: Shows Bounds Pointi to the Block Face that it intersects
                    spawn_cube_lines_rgba(world, point, float3_single(0.03f), 2, color_white, 0.01);
                    spawn_line3_alpha(world, point, block_position_face, 4, 0.01, color_gray);
                    spawn_cube_lines_rgba(world, block_position_face, float3_single(0.03f), 2, color_gray, 0.01);
                }
            }
        }
        // show correction vector
        if (!float3_equals(correction, float3_zero)) {
            float3 above = float3_add(position->value, (float3) { 0, bounds->value.y, 0 });
            spawn_line3_alpha(world, above, float3_add(above, correction), 4, 0.01, dbg_color_solid);
            spawn_cube_lines_rgba(world, float3_add(above, correction), float3_single(0.03f), 2, dbg_color_solid, 0.01);
        }
    }
} zox_sys_end(TerrainIntersectDebugSystem);
