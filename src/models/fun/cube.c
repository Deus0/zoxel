void voctree_fill_cube(VoxelNode* voctree, byte ndepth, byte value, byte3 position, byte3 size) {
    byte3 tsize = byte3_single(powers_of_two_byte[ndepth]);
    byte3 pos;
    for (pos.x = position.x; pos.x < position.x + size.x; pos.x++) {
        for (pos.y = position.y; pos.y < position.y + size.y; pos.y++) {
            for (pos.z = position.z; pos.z < position.z + size.z; pos.z++) {
                if (pos.x >= tsize.x || pos.y >= tsize.y || pos.z >= tsize.z) {
                    continue;
                }
                set_VoxelNode(voctree, ndepth, pos, value, 0);
            }
        }
    }
}
