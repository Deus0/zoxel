void voctree_paint(VoxelNode* voctree, byte ndepth, byte value, byte3 pp, byte3 ps) {
    byte vlength = powers_of_two_byte[ndepth];
    // byte3 tsize = byte3_single(vlength);
    byte3 vmax = (byte3) {
        int_clamp(pp.x + ps.x + 1, 0, 255),
        int_clamp(pp.y + ps.y + 1, 0, 255),
        int_clamp(pp.z + ps.z + 1, 0, 255)
    };
    byte3 position;
    for (position.x = pp.x; position.x < vmax.x; position.x++) {
        for (position.y = pp.y; position.y < vmax.y; position.y++) {
            for (position.z = pp.z; position.z < vmax.z; position.z++) {
                if (position.x >= vlength || position.y >= vlength || position.z >= vlength) {
                    continue;
                }
                byte voxel = getv_VoxelNode(voctree, ndepth, position);
                // can't paint air!
                if (!voxel) {
                    continue;
                }
                set_VoxelNode(voctree, ndepth, position, value);
            }
        }
    }
}
