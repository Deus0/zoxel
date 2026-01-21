void voctree_fill_sphere(VoxelNode* voctree, byte ndepth, byte value, byte3 position, byte radius) {

    byte3 tsize = byte3_single(powers_of_two_byte[ndepth]);

    byte3 pos;
    for (pos.x = 0; pos.x < tsize.x; pos.x++) {
        for (pos.y = 0; pos.y < tsize.y; pos.y++) {
            for (pos.z = 0; pos.z < tsize.z; pos.z++) {

                byte distance = byte3_distance(pos, position);

                if (distance > radius) {
                    continue;
                }

                fill_voctree(voctree, ndepth, pos, value, 0);
            }
        }
    }
}