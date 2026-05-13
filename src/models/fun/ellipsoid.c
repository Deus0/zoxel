void voctree_fill_ellipsoid(VoxelNode* voctree, byte ndepth, byte value, byte3 center, byte3 size) {
    byte3 tsize = byte3_single(powers_of_two_byte[ndepth]);
    // for every position in grid we iterate
    byte3 pposition;
    for (pposition.x = 0; pposition.x < tsize.x; pposition.x++) {
        for (pposition.y = 0; pposition.y < tsize.y; pposition.y++) {
            for (pposition.z = 0; pposition.z < tsize.z; pposition.z++) {
                int dx = (int)pposition.x - (int) center.x;
                int dy = (int)pposition.y - (int) center.y;
                int dz = (int)pposition.z - (int) center.z;
                int rx = (int) size.x / 2;
                int ry = (int) size.y / 2;
                int rz = (int) size.z / 2;
                // Ellipsoid test without division:
                // dx²*ry²*rz² + dy²*rx²*rz² + dz²*rx²*ry² <= rx²*ry²*rz²
                int dx2 = dx * dx;
                int dy2 = dy * dy;
                int dz2 = dz * dz;
                int rx2 = rx * rx;
                int ry2 = ry * ry;
                int rz2 = rz * rz;
                int lhs =
                    dx2 * ry2 * rz2 +
                    dy2 * rx2 * rz2 +
                    dz2 * rx2 * ry2;
                int rhs = rx2 * ry2 * rz2;
                if (lhs > rhs) {
                    continue;
                }
                set_VoxelNode(voctree, ndepth, pposition, value, 0);
            }
        }
    }
}
