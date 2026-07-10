static inline bool in_bounds(byte3 p, byte size) {
    return p.x < size && p.y < size && p.z < size;
}

// TODO: Move to octree macros
static inline void set_voxel_safe(VoxelNode *tree, byte3 p, byte depth, byte v) {
    if (in_bounds(p, octree_size(depth))) {
        set_VoxelNode(tree, depth, p, v, 0);
    }
}

void build_vox_flower_patch(VoxelNode* voctree, byte depth, byte2 stem_range, byte2 petal_range, byte black_voxel) {
    if (depth == 0) {
        set_VoxelNode(voctree, depth, byte3_zero, black_voxel, 0);
        return;
    }
    byte spawn_count = rand_range(4, 12); // 4 + rand() % 8; // 32
    byte size = octree_size(depth); //  powers_of_two_byte[depth];
    byte3 pos;
    byte3 base = byte3_zero;
    byte2 stem_heights = (byte2) { size / 6, size / 2 };
    byte margins = size / 8;
    for (int i = 0; i < spawn_count; i++) {
        // random flower base
        base.x = rand_range(margins, size - margins); // rand() % size;
        base.y = 0; // keep them grounded
        base.z = rand_range(margins, size - margins);
        byte stem_height = rand_range(stem_heights.x, stem_heights.y); // 2 + rand() % 7;
        byte petal_type = rand_range(0, 4); // rand() % 4;
        if (size == 2) {
            petal_type = 3;
        }
        // stem
        for (byte h = 0; h < stem_height; h++) {
            pos = base;
            pos.y += h;
            byte c = stem_range.x + rand() % (stem_range.y - stem_range.x);
            set_voxel_safe(voctree, pos, depth, c);
        }
        // petals
        pos.y = base.y + stem_height;
        byte flower_color = petal_range.x + rand() % (petal_range.y - petal_range.x);
        switch (petal_type) {
            case 0: // cross pattern
                set_voxel_safe(voctree, pos, depth, flower_color);
                set_voxel_safe(voctree, (byte3) { pos.x + 1, pos.y, pos.z }, depth, flower_color);
                set_voxel_safe(voctree, (byte3) { pos.x - 1, pos.y, pos.z }, depth, flower_color);
                set_voxel_safe(voctree, (byte3) { pos.x, pos.y, pos.z + 1 }, depth, flower_color);
                set_voxel_safe(voctree, (byte3) { pos.x, pos.y, pos.z - 1 }, depth, flower_color);
                break;
            case 1: // cluster ball
                for (sbyte dx = -1; dx <= 1; dx++) {
                    for (sbyte dz = -1; dz <= 1; dz++) {
                        if (rand() % 2) {
                            byte3 petal = { pos.x + dx, pos.y, pos.z + dz };
                            set_voxel_safe(voctree, petal, depth, flower_color);
                        }
                    }
                }
                break;
            case 2: // flat ring
                for (byte i = 0; i < 6; i++) {
                    sbyte dx = (i & 1) ? 1 : -1;
                    sbyte dz = (i & 2) ? 1 : -1;
                    byte3 ring = { pos.x + dx, pos.y, pos.z + dz };
                    set_voxel_safe(voctree, ring, depth, flower_color);
                }
                break;
            case 3: // withered single
                set_voxel_safe(voctree, pos, depth, black_voxel);
                break;
        }
    }
}
