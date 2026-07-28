void voctree_fill_sphere(VoxelNode* voctree, byte ndepth, byte value, byte3 center, byte radius) {
    byte3 tsize = byte3_single(powers_of_two_byte[ndepth]);
    byte3 pposition;
    for (pposition.x = 0; pposition.x < tsize.x; pposition.x++) {
        for (pposition.y = 0; pposition.y < tsize.y; pposition.y++) {
            for (pposition.z = 0; pposition.z < tsize.z; pposition.z++) {
                byte distance = byte3_distance(pposition, center);
                if (distance > radius) {
                    continue;
                }
                set_voxel_safe(voctree, ndepth, pposition, value);
            }
        }
    }
}
