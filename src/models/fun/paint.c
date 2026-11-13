// paints solid voxels in a box (pp the corner, ps its size, as voctree_fill_cube), air stays air
//  (it covered ps + 1 a side, eyes a voxel too wide: tst model_nodes)
void voctree_paint(VoxelNode* voctree, byte ndepth, byte value, byte3 pp, byte3 ps) {
    byte vlength = powers_of_two_byte[ndepth];
    byte3 vmax = (byte3) {
        int_clamp(pp.x + ps.x, 0, 255),
        int_clamp(pp.y + ps.y, 0, 255),
        int_clamp(pp.z + ps.z, 0, 255)
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
                set_voxel_safe(voctree, ndepth, position, value);
            }
        }
    }
}
