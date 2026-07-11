static inline bool in_bounds(byte3 p, byte size) {
    return p.x < size && p.y < size && p.z < size;
}

// TODO: Move to octree macros
static inline void set_voxel_safe(VoxelNode *tree, byte3 p, byte depth, byte v) {
    if (in_bounds(p, octree_size(depth))) {
        set_VoxelNode(tree, depth, p, v);
    } else {
        zox_logw("Position [%ix%ix%i] out of B [%i]", p.x, p.y, p.z, depth);
    }
}

void build_vox_flower_patch(VoxelNode* voctree, byte depth, byte2 stem_range, byte2 petal_range, byte black_voxel) {
    if (depth == 0) {
        set_VoxelNode(voctree, depth, byte3_zero, black_voxel);
        return;
    }
    byte is_stalks_only = depth <= 2;
    float stem_max_height = 0.8f;
    byte size = octree_size(depth);
    byte spawn_count = rand_range(1 + (size * size) / 64, 1 + (size * size) / 16);
    byte2 stem_heights = (byte2) { size / 6, (byte) (size * stem_max_height) };
    byte margins = size / 8;
    if (!is_stalks_only && margins == 0) {
        margins = 1;
    }
    byte lhs = margins;
    byte rhs = size - margins;
    byte3 pos;
    byte3 base = byte3_zero;
    for (ushort i = 0; i < spawn_count; i++) {
        // Get Petal Type
        byte petal_type = is_stalks_only ? 3 : rand_range(0, 4);
        // random flower base
        base.x = rand_range(lhs, rhs);
        base.z = rand_range(lhs, rhs);
        // keep them grounded
        base.y = 0;
        byte stem_height = rand_range(stem_heights.x, stem_heights.y);
        // NOTE: Limit Petal type at lower depths
        // stem
        pos = base;
        for (byte h = 0; h < stem_height; h++) {
            pos.y = h;
            byte c = rand_range(stem_range.x, stem_range.y);
            set_voxel_safe(voctree, pos, depth, c);
        }
        // petals
        pos.y = base.y + stem_height;
        byte flower_color = rand_range(petal_range.x, petal_range.y);
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
