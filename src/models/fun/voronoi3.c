// critical for finding voronoi index from a position
static inline int voronoi_distance(
    byte3 point,
    byte3 point_2,
    byte length)
{
    int dx = (int) point.x - point_2.x;
    int dy = (int) point.y - point_2.y;
    int dz = (int) point.z - point_2.z;

    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    if (dz < 0) dz = -dz;

    int half = length >> 1;

    if (dx > half) dx = length - dx;
    if (dy > half) dy = length - dy;
    if (dz > half) dz = length - dz;

    return dx * dx + dy * dy + dz * dz;
}

static inline int get_closest_index3(
    byte3 point,
    byte3* points,
    int points_length,
    byte length)
{
    int smallest_distance = 0x7fffffff;
    int smallest_index = 0;

    for (int i = 0; i < points_length; i++) {
        int distance = voronoi_distance(
            point,
            points[i],
            length);

        if (distance < smallest_distance) {
            smallest_distance = distance;
            smallest_index = i;
        }
    }

    return smallest_index;
}

void voronoi3(
    VoxelNode* node,
    byte depth,
    byte2 voxel_range,
    byte black_voxel,
    byte vregions,
    uint* random_state)
{
    if (!vregions) {
        return;
    }

    byte length = powers_of_two[depth];
    int points_length = (length * 8) / 5;
    int voxels_length = length * length * length;

    byte3 points[points_length];
    byte regions[points_length];
    byte region_voxels[voxels_length];

    uint mask = (uint) length - 1;
    int point_closeness = (length * 2 + 4) / 5;

    for (int i = 0; i < points_length; i++) {
        byte3 point;

        for (int count = 0;; count++) {
            uint random = noise_rand(random_state);

            point.x = (byte) (random & mask);
            point.y = (byte) ((random >> 8) & mask);
            point.z = (byte) ((random >> 16) & mask);

            byte too_close = 0;

            for (int j = i - 1; j >= 0; j--) {
                byte3 point_2 = points[j];

                int dx = (int) point.x - point_2.x;
                int dy = (int) point.y - point_2.y;
                int dz = (int) point.z - point_2.z;

                if (dx < 0) dx = -dx;
                if (dy < 0) dy = -dy;
                if (dz < 0) dz = -dz;

                if (dx < point_closeness &&
                    dy < point_closeness &&
                    dz < point_closeness)
                {
                    too_close = 1;
                    break;
                }
            }

            if (!too_close || count >= 128) {
                break;
            }
        }

        points[i] = point;
        regions[i] = (byte)(
            noise_rand(random_state) % vregions);
    }

    byte3 position;

    // Build Voronoi regions.
    for (position.x = 0; position.x < length; position.x++) {
        for (position.y = 0; position.y < length; position.y++) {
            for (position.z = 0; position.z < length; position.z++) {
                int region_index = get_closest_index3(
                    position,
                    points,
                    points_length,
                    length);

                int index = byte3_array_indexl(
                    position,
                    length);

                region_voxels[index] = regions[region_index];
            }
        }
    }

    // Darken boundaries and write voxels in the same pass.
    for (position.x = 0; position.x < length; position.x++) {
        for (position.y = 0; position.y < length; position.y++) {
            for (position.z = 0; position.z < length; position.z++) {
                int index = byte3_array_indexl(
                    position,
                    length);

                byte region_voxel = region_voxels[index];
                byte darken = 0;

                if (position.x + 1 < length) {
                    int index_right = index + length * length;
                    darken |= region_voxel != region_voxels[index_right];
                }

                if (position.y + 1 < length) {
                    int index_up = index + length;
                    darken |= region_voxel != region_voxels[index_up];
                }

                if (position.z + 1 < length) {
                    darken |= region_voxel != region_voxels[index + 1];
                }

                byte value = black_voxel;

                if (!darken) {
                    value =
                        voxel_range.x +
                        (region_voxel %
                        (voxel_range.y - voxel_range.x));
                }

                set_VoxelNode(
                    node,
                    depth,
                    position,
                    value);
            }
        }
    }
}

/*#define closest_dist_check_macro(x, y, z) {\
    const float dist = float3_distance(point_f3, float3_add(region_point, (float3) { x, y, z }));\
    if (dist < smallestDistance) {\
        smallestDistance = dist;\
        smallestIndex = i;\
    }\
}

int get_closest_index3(
    byte3 point,
    byte3 *points,
    int points_length,
    byte length)
{
    float3 point_f3 = byte3_to_float3(point);
    float smallestDistance = 100000; // float.MaxValue;
    float smallestIndex = 0;
    for (int i = 0; i < points_length; i++) {
        float3 region_point = byte3_to_float3(points[i]);
        for (float x = -1; x <= 1; x++) {
            for (float y = -1; y <= 1; y++) {
                for (float z = -1; z <= 1; z++) {
                    closest_dist_check_macro(x * length, y * length, z * length)
                }
            }
        }
    }
    return smallestIndex;
}

void voronoi3(
    VoxelNode *node,
    byte depth,
    byte2 voxel_range,
    byte black_voxel,
    byte vregions)
{
    if (!vregions) {
        return;
    }
    byte length = powers_of_two[depth];
    // const byte vregions = 64;
    int points_length = (int) (length * 1.6f);
    int voxels_length = length * length * length;
    float pointCloseness = length / 5;
    byte3 position = byte3_zero;
    byte3 points[points_length];
    byte regions[points_length];
    byte region_voxels[voxels_length];
    for (int i = 0; i < points_length; i++) {
        points[i] = byte3_zero;
    }
    for (int i = 0; i < points_length; i++) {
        byte3 point = (byte3) {
            rand() % length,
            rand() % length,
            rand() % length
        };
        int count = 0;
        while (count <= 128) {
            point = (byte3) {
                rand() % length,
                rand() % length,
                rand() % length
            };
            byte isTooClose = 0;
            for (int j = i - 1; j >= 0; j--) {
                byte3 point2 = points[j];
                if (    point.x + pointCloseness > point2.x - pointCloseness &&
                        point.x - pointCloseness < point2.x + pointCloseness &&
                        point.y + pointCloseness > point2.y - pointCloseness &&
                        point.y - pointCloseness < point2.y + pointCloseness &&
                        point.z + pointCloseness > point2.z - pointCloseness &&
                        point.z - pointCloseness < point2.z + pointCloseness)
                {
                    isTooClose = 1;
                    break;
                }
            }
            if (!isTooClose) {
                break;
            }
            count++;
        }
        points[i] = point;
        regions[i] = rand() % vregions;
    }
    for (position.x = 0; position.x < length; position.x++) {
        for (position.y = 0; position.y < length; position.y++) {
            for (position.z = 0; position.z < length; position.z++) {
                int region_index = get_closest_index3(position, points, points_length, length);
                int index = byte3_array_indexl(position, length);
                region_voxels[index] = regions[region_index];
            }
        }
    }
    byte is_darken[voxels_length];
    for (position.x = 0; position.x < length; position.x++) {
        for (position.y = 0; position.y < length; position.y++) {
            for (position.z = 0; position.z < length; position.z++) {
                const int index = byte3_array_indexl(position, length);
                const byte voxel = region_voxels[index];
                byte is_voxel_up = 0;
                byte is_voxel_right = 0;
                byte is_voxel_front = 0;
                // right
                if (position.x != length - 1 || position.y == length - 1 || position.z == length - 1) {
                    byte3 position_right = byte3_right(position);
                    if (position_right.x == length) {
                        position_right.x = 0;
                    }
                    int index_right = byte3_array_indexl(position_right, length);
                    is_voxel_right = voxel != region_voxels[index_right];
                }
                // up
                if (position.y != length - 1 || position.x == length - 1 || position.z == length - 1) {
                    byte3 position_up = byte3_up(position);
                    if (position_up.y == length) {
                        position_up.y = 0;
                    }
                    int index_up = byte3_array_indexl(position_up, length);
                    is_voxel_up = voxel != region_voxels[index_up];
                }
                // back
                if (position.z != length - 1 || position.x == length - 1 || position.y == length - 1) {
                    byte3 position_forward = byte3_front(position);
                    if (position_forward.z == length) {
                        position_forward.z = 0;
                    }
                    int index_forward = byte3_array_indexl(position_forward, length);
                    is_voxel_front = voxel != region_voxels[index_forward];
                }
                // darken
                is_darken[index] = is_voxel_up || is_voxel_right || is_voxel_front;
            }
        }
    }
    // now darken in another pass
    for (position.x = 0; position.x < length; position.x++) {
        for (position.y = 0; position.y < length; position.y++) {
            for (position.z = 0; position.z < length; position.z++) {
                int index = byte3_array_indexl(position, length);
                if (is_darken[index]) {
                    region_voxels[index] = vregions;
                }
            }
        }
    }
    // const byte2 set_voxel_black = (byte2) { black_voxel, depth };
    for (position.x = 0; position.x < length; position.x++) {
        for (position.y = 0; position.y < length; position.y++) {
            for (position.z = 0; position.z < length; position.z++) {
                const int index = byte3_array_indexl(position, length);
                const byte region_voxel = region_voxels[index];
                byte value = black_voxel;
                //  set_voxel = (byte2) { black_voxel, depth };
                if (region_voxel != vregions) {
                    value = voxel_range.x + (region_voxel % (voxel_range.y - voxel_range.x));
                }
                set_VoxelNode(node, depth, position, value);
            }
        }
    }
}
*/