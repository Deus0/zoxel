void voctree_paint(VoxelNode* voctree, byte ndepth, byte value, byte3 pp, byte3 ps) {

    byte3 tsize = byte3_single(powers_of_two_byte[ndepth]);

    byte3 pos;
    for (pos.x = pp.x; pos.x < pp.x + ps.x; pos.x++) {
        for (pos.y = pp.y; pos.y < pp.y + ps.y; pos.y++) {
            for (pos.z = pp.z; pos.z < pp.z + ps.z; pos.z++) {

                if (pos.x >= tsize.x || pos.y >= tsize.y || pos.z >= tsize.z) {
                    continue;
                }

                paint_voctree(voctree, ndepth, pos, value, 0);
            }
        }
    }
}
