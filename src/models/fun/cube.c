// fills a box, clipped to the octree: ints, so a box past 255 (i.e. a byte that wrapped) can't loop forever
void voctree_fill_cube(VoxelNode* voctree, byte ndepth, byte value, byte3 position, byte3 size) {
    const int length = powers_of_two_byte[ndepth];
    const int3 end = {
        int_min(position.x + size.x, length),
        int_min(position.y + size.y, length),
        int_min(position.z + size.z, length) };
    for (int x = position.x; x < end.x; x++) {
        for (int y = position.y; y < end.y; y++) {
            for (int z = position.z; z < end.z; z++) {
                set_voxel_safe(voctree, ndepth, (byte3) { x, y, z }, value);
            }
        }
    }
}
