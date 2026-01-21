void voctree_fill_ellipsoid(VoxelNode* voctree, byte ndepth, byte value, byte3 position, byte3 size) {

    byte3 tsize = byte3_single(powers_of_two_byte[ndepth]);

    byte3 pos;
    for (pos.x = 0; pos.x < tsize.x; pos.x++) {
        for (pos.y = 0; pos.y < tsize.y; pos.y++) {
            for (pos.z = 0; pos.z < tsize.z; pos.z++) {

                int dx = (int)pos.x - (int)position.x;
                int dy = (int)pos.y - (int)position.y;
                int dz = (int)pos.z - (int)position.z;

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

                fill_voctree(voctree, ndepth, pos, value, 0);
            }
        }
    }
}