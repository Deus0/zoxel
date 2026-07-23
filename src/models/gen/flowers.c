void build_vox_flowers(VoxelNode* voctree, byte depth, byte2 stem_range, byte2 petal_range, byte black_voxel) {
    if (depth == 0) {
        set_VoxelNode(voctree, depth, byte3_zero, black_voxel);
        return;
    }
    byte is_stalks_only = depth <= 2;
    byte size = octree_size(depth);
    byte spawn_count = rand_range(1 + (size * size) / 64, 1 + (size * size) / 16);
    byte2 stem_heights = (byte2) { size / 2, size };
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
            set_voxel_safe(voctree, depth, pos, c);
        }
        // petals
        pos.y = base.y + stem_height;
        byte flower_color = rand_range(petal_range.x, petal_range.y);
        switch (petal_type) {
            case 0: // cross pattern
                set_voxel_safe(voctree, depth, pos, flower_color);
                set_voxel_safe(voctree, depth, (byte3) { pos.x + 1, pos.y, pos.z }, flower_color);
                set_voxel_safe(voctree, depth, (byte3) { pos.x - 1, pos.y, pos.z }, flower_color);
                set_voxel_safe(voctree, depth, (byte3) { pos.x, pos.y, pos.z + 1 }, flower_color);
                set_voxel_safe(voctree, depth, (byte3) { pos.x, pos.y, pos.z - 1 }, flower_color);
                break;
            case 1: // cluster ball
                for (sbyte dx = -1; dx <= 1; dx++) {
                    for (sbyte dz = -1; dz <= 1; dz++) {
                        if (rand() % 2) {
                            byte3 petal = { pos.x + dx, pos.y, pos.z + dz };
                            set_voxel_safe(voctree, depth, petal, flower_color);
                        }
                    }
                }
                break;
            case 2: // flat ring
                for (byte i = 0; i < 6; i++) {
                    sbyte dx = (i & 1) ? 1 : -1;
                    sbyte dz = (i & 2) ? 1 : -1;
                    byte3 ring = { pos.x + dx, pos.y, pos.z + dz };
                    set_voxel_safe(voctree, depth, ring, flower_color);
                }
                break;
            case 3: // withered single
                set_voxel_safe(voctree, depth, pos, black_voxel);
                break;
        }
    }
}
