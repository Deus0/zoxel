// fills an ellipsoid: centre ± size / 2 a side (2r + 1 voxels across)
//  - radii are half a voxel more than size / 2 (r + 0.5): the poles are caps, not a lone voxel poking out
//      (tst model_nodes), and it stays inside centre ± r
//  - doubled coordinates, so whole numbers: (2d / (2r + 1))² summed <= 1, 64 bit (255^6 overflows 32)
void voctree_fill_ellipsoid(VoxelNode* voctree, byte ndepth, byte value, byte3 center, byte3 size) {
    const short length = octree_size(ndepth);
    const uint64_t rx = 2 * (size.x / 2) + 1;
    const uint64_t ry = 2 * (size.y / 2) + 1;
    const uint64_t rz = 2 * (size.z / 2) + 1;
    const uint64_t rx2 = rx * rx, ry2 = ry * ry, rz2 = rz * rz;
    const uint64_t rhs = rx2 * ry2 * rz2;
    byte3 p;
    for (p.x = 0; p.x < length; p.x++)
    for (p.y = 0; p.y < length; p.y++)
    for (p.z = 0; p.z < length; p.z++) {
        const int64_t dx = 2 * ((int) p.x - center.x);
        const int64_t dy = 2 * ((int) p.y - center.y);
        const int64_t dz = 2 * ((int) p.z - center.z);
        const uint64_t lhs = (uint64_t) (dx * dx) * ry2 * rz2 +
            (uint64_t) (dy * dy) * rx2 * rz2 +
            (uint64_t) (dz * dz) * rx2 * ry2;
        if (lhs > rhs) {
            continue;
        }
        set_voxel_safe(voctree, ndepth, p, value);
    }
}
