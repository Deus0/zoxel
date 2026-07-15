void voctree_fill_ellipsoid(VoxelNode* voctree, byte ndepth, byte value, byte3 center, byte3 size) {
    short length = octree_size(ndepth);
    byte3 tsize = byte3_single(length);
    // for every position in grid we iterate
    byte3 pposition;
    for (pposition.x = 0; pposition.x < tsize.x; pposition.x++)
    for (pposition.y = 0; pposition.y < tsize.y; pposition.y++)
    for (pposition.z = 0; pposition.z < tsize.z; pposition.z++) {
        sbyte dx = pposition.x - center.x;
        sbyte dy = pposition.y - center.y;
        sbyte dz = pposition.z - center.z;
        sbyte rx = size.x / 2;
        sbyte ry = size.y / 2;
        sbyte rz = size.z / 2;
        // Ellipsoid test without division:
        // dx²*ry²*rz² + dy²*rx²*rz² + dz²*rx²*ry² <= rx²*ry²*rz²
        uint dx2 = dx * dx;
        uint dy2 = dy * dy;
        uint dz2 = dz * dz;
        uint rx2 = rx * rx;
        uint ry2 = ry * ry;
        uint rz2 = rz * rz;
        uint lhs =  dx2 * ry2 * rz2 +
                    dy2 * rx2 * rz2 +
                    dz2 * rx2 * ry2;
        uint rhs =   rx2 * ry2 * rz2;
        if (lhs > rhs) {
            continue;
        }
        set_VoxelNode(voctree, ndepth, pposition, value);
    }
}
