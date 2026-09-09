// Collision detection for voxel terrain.
// Kept compatible with the existing inputs and outputs, but simplified.
// The actual system now tests one axis at a time against the moving face of the AABB.

/*static inline float get_axis_f3(const float3 v, const byte axis) {
    switch (axis) {
        case zox_axis_x: return v.x;
        case zox_axis_y: return v.y;
        default: return v.z;
    }
}

static inline float3 float3_sub3(const float3 a, const float3 b) {
    return (float3){ a.x - b.x, a.y - b.y, a.z - b.z };
}

static inline float3 float3_add3(const float3 a, const float3 b) {
    return (float3){ a.x + b.x, a.y + b.y, a.z + b.z };
}

static inline byte chunk_voxel_solid(
    const ChunkLinks *chunks,
    const byte terrain_depth,
    const float terrain_scale,
    const byte *block_collisions,
    const int3 voxel_position,
    float *chunk_scale_out
) {
    const int3 chunk_dimensions = int3_single(powers_of_two[terrain_depth]);
    const int3 chunk_position = block_position_to_chunk_position(voxel_position, terrain_depth);
    const entity chunk = int3_hashmap_get(chunks->value, chunk_position);
    if (!zox_valid(chunk)) {
        return 0;
    }
    zox_geter(chunk, VoxelNode, node)
    if (!node) {
        return 0;
    }
    zox_geter_value(chunk, NodeDepth, byte, chunk_depth)
    const byte ddepth = terrain_depth - chunk_depth + 1;
    const int3 voxel_position2 = int3_div1(voxel_position, (int)ddepth);
    const byte3 chunk_size = byte3_single(powers_of_two[chunk_depth]);
    const byte3 voxel_positionl = get_positionl_byte3(voxel_position2, chunk_size);
    if (!byte3_in_bounds(voxel_positionl, chunk_size)) {
        return 0;
    }
    const byte voxel = getv_VoxelNode(node, terrain_depth, voxel_positionl);
    if (chunk_scale_out) {
        *chunk_scale_out = terrain_scale / ((float)ddepth);
    }
    return block_collisions[voxel] != 0;
}

static inline void collide_axis_face(
    const ChunkLinks *chunks,
    const byte terrain_depth,
    const float terrain_scale,
    const byte *block_collisions,
    const float3 position,
    const float3 last_position,
    const float3 bounds,
    const byte axis,
    byte *collided_d,
    float *distance_d
) {
    if (*collided_d) {
        return;
    }
    const float p = get_axis_f3(position, axis);
    const float lp = get_axis_f3(last_position, axis);
    const float delta = p - lp;

    if (delta == 0.0f) {
        return;
    }

    const byte is_positive = delta > 0.0f;
    const byte side = is_positive ? 1 : 2;
    const float sign = is_positive ? 1.0f : -1.0f;
    const float eps = terrain_scale * 0.0001f + 0.000001f;

    const float3 aabb_min = float3_sub3(position, bounds);
    const float3 aabb_max = float3_add3(position, bounds);
    const float3 aabb_max_adj = (float3){
        aabb_max.x - eps,
        aabb_max.y - eps,
        aabb_max.z - eps
    };

    const int3 min_vox = real_position_to_block_position(aabb_min, terrain_scale);
    const int3 max_vox = real_position_to_block_position(aabb_max_adj, terrain_scale);

    const float face = p + (get_axis_f3(bounds, axis) * sign);
    const int face_vox = real_position_to_block_position1(face + (eps * sign), terrain_scale);

    int3 query = min_vox;
    float hit_scale = terrain_scale;

    switch (axis) {
        case zox_axis_x:
            query.x = face_vox;
            for (int y = min_vox.y; y <= max_vox.y; y++) {
                for (int z = min_vox.z; z <= max_vox.z; z++) {
                    query.y = y;
                    query.z = z;
                    if (chunk_voxel_solid(chunks, terrain_depth, terrain_scale, block_collisions, query, &hit_scale)) {
                        *collided_d = side;
                        *distance_d = get_distance_to_voxel_grid(p, side, hit_scale);
                        return;
                    }
                }
            }
            break;

        case zox_axis_y:
            query.y = face_vox;
            for (int x = min_vox.x; x <= max_vox.x; x++) {
                for (int z = min_vox.z; z <= max_vox.z; z++) {
                    query.x = x;
                    query.z = z;
                    if (chunk_voxel_solid(chunks, terrain_depth, terrain_scale, block_collisions, query, &hit_scale)) {
                        *collided_d = side;
                        *distance_d = get_distance_to_voxel_grid(p, side, hit_scale);
                        return;
                    }
                }
            }
            break;

        default:
            query.z = face_vox;
            for (int x = min_vox.x; x <= max_vox.x; x++) {
                for (int y = min_vox.y; y <= max_vox.y; y++) {
                    query.x = x;
                    query.y = y;
                    if (chunk_voxel_solid(chunks, terrain_depth, terrain_scale, block_collisions, query, &hit_scale)) {
                        *collided_d = side;
                        *distance_d = get_distance_to_voxel_grid(p, side, hit_scale);
                        return;
                    }
                }
            }
            break;
    }
}

void collide_with_chunk(
    ecs* world,
    const ChunkLinks* chunks,
    byte terrain_depth,
    float terrain_scale,
    const byte *block_collisions,
    int3 voxel_position,
    byte axis_d,
    float position_d,
    float position_last_d,
    float offset_d,
    byte *collided_d,
    float *distance_d,
    byte is_negative
) {
    if (*collided_d) {
        return;
    }

    position_d += offset_d;
    position_last_d += offset_d;

    const int position_vox_d = real_position_to_block_position1(position_d, terrain_scale);
    const int position_vox_last_d = real_position_to_block_position1(position_last_d, terrain_scale);

    if (position_vox_d == position_vox_last_d) {
        return;
    }

    int3 query = voxel_position;
    int3_set_d(&query, axis_d, position_vox_d);

    float hit_scale = terrain_scale;
    if (!chunk_voxel_solid(chunks, terrain_depth, terrain_scale, block_collisions, query, &hit_scale)) {
        return;
    }

    *collided_d = 1 + is_negative;
    *distance_d = get_distance_to_voxel_grid(position_d, *collided_d, hit_scale);
}

void collide_with_chunk_d2(
    ecs *world,
    const ChunkLinks *chunks,
    const byte terrain_depth,
    const float terrain_scale,
    const byte *block_collisions,
    int3 voxel_position,
    const byte axis_d1,
    float position_d1,
    float position_last_d1,
    const float offset_d1,
    byte *collided_d1,
    float *distance_d1,
    const byte is_negative1,
    const byte axis_d2,
    float position_d2,
    float position_last_d2,
    const float offset_d2,
    byte *collided_d2,
    float *distance_d2,
    const byte is_negative2
) {
    if (*collided_d1 || *collided_d2) {
        return;
    }

    position_d1 += offset_d1;
    position_last_d1 += offset_d1;
    position_d2 += offset_d2;
    position_last_d2 += offset_d2;

    const int v1 = real_position_to_block_position1(position_d1, terrain_scale);
    const int vl1 = real_position_to_block_position1(position_last_d1, terrain_scale);
    const int v2 = real_position_to_block_position1(position_d2, terrain_scale);
    const int vl2 = real_position_to_block_position1(position_last_d2, terrain_scale);

    if (v1 == vl1 && v2 == vl2) {
        return;
    }

    const int d1 = int_abs(v1 - vl1);
    const int d2 = int_abs(v2 - vl2);

    if (d1 > d2) {
        collide_with_chunk(
            world, chunks, terrain_depth, terrain_scale, block_collisions,
            voxel_position, axis_d1, position_d1, position_last_d1, 0.0f,
            collided_d1, distance_d1, is_negative1
        );
    } else {
        collide_with_chunk(
            world, chunks, terrain_depth, terrain_scale, block_collisions,
            voxel_position, axis_d2, position_d2, position_last_d2, 0.0f,
            collided_d2, distance_d2, is_negative2
        );
    }
}

void collide_with_chunk_d3(
    ecs *world,
    const ChunkLinks *chunks,
    const byte terrain_depth,
    const float scale,
    const byte *block_collisions,
    int3 voxel_position,
    const byte axis_d1,
    float position_d1,
    float position_last_d1,
    const float offset_d1,
    byte *collided_d1,
    float *distance_d1,
    const byte is_negative1,
    const byte axis_d2,
    float position_d2,
    float position_last_d2,
    const float offset_d2,
    byte *collided_d2,
    float *distance_d2,
    const byte is_negative2,
    const byte axis_d3,
    float position_d3,
    float position_last_d3,
    const float offset_d3,
    byte *collided_d3,
    float *distance_d3,
    const byte is_negative3
) {
    if (*collided_d1 || *collided_d2 || *collided_d3) {
        return;
    }

    position_d1 += offset_d1;
    position_last_d1 += offset_d1;
    position_d2 += offset_d2;
    position_last_d2 += offset_d2;
    position_d3 += offset_d3;
    position_last_d3 += offset_d3;

    const int v1 = real_position_to_block_position1(position_d1, scale);
    const int vl1 = real_position_to_block_position1(position_last_d1, scale);
    const int v2 = real_position_to_block_position1(position_d2, scale);
    const int vl2 = real_position_to_block_position1(position_last_d2, scale);
    const int v3 = real_position_to_block_position1(position_d3, scale);
    const int vl3 = real_position_to_block_position1(position_last_d3, scale);

    if (v1 == vl1 && v2 == vl2 && v3 == vl3) {
        return;
    }

    const int d1 = int_abs(v1 - vl1);
    const int d2 = int_abs(v2 - vl2);
    const int d3 = int_abs(v3 - vl3);

    if (d1 > d2 && d1 > d3) {
        collide_with_chunk(
            world, chunks, terrain_depth, scale, block_collisions,
            voxel_position, axis_d1, position_d1, position_last_d1, 0.0f,
            collided_d1, distance_d1, is_negative1
        );
    } else if (d2 > d1 && d2 > d3) {
        collide_with_chunk(
            world, chunks, terrain_depth, scale, block_collisions,
            voxel_position, axis_d2, position_d2, position_last_d2, 0.0f,
            collided_d2, distance_d2, is_negative2
        );
    } else {
        collide_with_chunk(
            world, chunks, terrain_depth, scale, block_collisions,
            voxel_position, axis_d3, position_d3, position_last_d3, 0.0f,
            collided_d3, distance_d3, is_negative3
        );
    }
}

zox_sys2(CollisionDetectSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TerrainLink);
    zox_sys_in(Bounds3D);
    zox_sys_in(Position3D);
    zox_sys_in(LastPosition3D);
    zox_sys_out(Collision);
    zox_sys_out(CollisionDistance);

    const BlockLinks *voxels = get_first_terrain_voxels(world, TerrainLink_, it->count);
    if (!voxels) {
        zox_logw("No Terrain Detected");
        return;
    }

    byte block_collisions[voxels->length + 1];
    get_block_collisions(world, voxels, block_collisions);

    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TerrainLink, link);
        zox_sys_i(Bounds3D, bounds3D);
        zox_sys_i(Position3D, position3D);
        zox_sys_i(LastPosition3D, lastPosition3D);
        zox_sys_o(Collision, collision);
        zox_sys_o(CollisionDistance, collisionDistance);

        if (!zox_valid(link->value)) {
            zox_logw("Terrain Invalid");
            continue;
        }

        zox_geter(link->value, ChunkLinks, chunks);
        zox_geter_value(link->value, NodeDepth, byte, terrain_depth);
        zox_geter_value(link->value, BlockScale, float, terrain_scale);

        if (!chunks || collision->value) {
            continue;
        }

        const float3 collision_point_real = position3D->value;
        const float3 position_last = lastPosition3D->value;
        const float3 bounds = bounds3D->value;

        byte3 did_collide = byte3_zero;
        float3 collision_distance = (float3){ 0.0f, 0.0f, 0.0f };

        collide_axis_face(
            chunks,
            terrain_depth,
            terrain_scale,
            block_collisions,
            collision_point_real,
            position_last,
            bounds,
            zox_axis_x,
            &did_collide.x,
            &collision_distance.x
        );

        collide_axis_face(
            chunks,
            terrain_depth,
            terrain_scale,
            block_collisions,
            collision_point_real,
            position_last,
            bounds,
            zox_axis_y,
            &did_collide.y,
            &collision_distance.y
        );

        collide_axis_face(
            chunks,
            terrain_depth,
            terrain_scale,
            block_collisions,
            collision_point_real,
            position_last,
            bounds,
            zox_axis_z,
            &did_collide.z,
            &collision_distance.z
        );

        if (did_collide.x || did_collide.y || did_collide.z) {
            collisionDistance->value = collision_distance;
            collision->value =
            ((did_collide.x & 0x3) << 0) |
            ((did_collide.y & 0x3) << 2) |
            ((did_collide.z & 0x3) << 4);
        }
    }
} zox_sys_end(CollisionDetectSystem);
*/
