void build_vox_wood(VoxelNode *voctree, byte depth, byte2 wood, byte2 bark) {
    byte vlength = powers_of_two_byte[depth];
    byte3 size = byte3_single(vlength);
    byte3 pos;
    byte bark_thickness = vlength > 16 ? 2 : 1;
    byte ring_period = vlength / 6;
    if (ring_period < 2) ring_period = 2;
    byte wood_range = wood.y - wood.x + 1;
    byte bark_range = bark.y - bark.x + 1;
    if (wood_range == 0) wood_range = 1;
    if (bark_range == 0) bark_range = 1;
    sbyte cx = size.x / 2;
    sbyte cz = size.z / 2;
    for (pos.y = 0; pos.y < size.y; pos.y++) {
        for (pos.x = 0; pos.x < size.x; pos.x++) {
            for (pos.z = 0; pos.z < size.z; pos.z++) {
                /* bark on XZ perimeter */
                if (pos.x < bark_thickness || pos.x >= size.x - bark_thickness ||
                    pos.z < bark_thickness || pos.z >= size.z - bark_thickness) {
                    byte v = bark.x + (rand() % bark_range);
                    /* vertical streaking */
                    if ((pos.y + rand()) & 1) {
                        if (v > bark.x) {
                            v--;
                        }
                    }
                    set_VoxelNode(voctree, depth, pos, v);
                    continue;
                }
                /* radial distance squared */
                sbyte dx = (sbyte)pos.x - cx;
                sbyte dz = (sbyte)pos.z - cz;
                byte r2 = (byte)(dx*dx + dz*dz);
                /* ring phase (density modulation, not hard bands) */
                byte ring = (r2 / ring_period) & 1;
                /* base wood color (always varied) */
                byte v = wood.x + (rand() % wood_range);
                /* ring density bias */
                if (ring) {
                    if (v > wood.x) v--;
                } else {
                    if (v < wood.y) v++;
                }
                /* vertical grain noise */
                if ((pos.y + rand()) & 1) {
                    if (v > wood.x) v--;
                }
                set_VoxelNode(voctree, depth, pos, v);
            }
        }
    }
}
