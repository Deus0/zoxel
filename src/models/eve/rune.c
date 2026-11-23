// a rune node: a seeded rune lit on each face's middle, its NodeVoxel painted on the first solid voxel in from the
//  face (through a window, onto the core), only on the frames shown while its block is on (ModelActive)
//  - a 5 x 5 glyph from the model's Seed (each realm its own): a spine down its middle, an arm, a short arm, a
//    diagonal or nothing off each row, mirrored left to right (rune like), each cell rune_cell sixteenths of the box
#define rune_size 5
#define rune_cell 1     // sixteenths of the box: 2 voxels a cell at 32, 1 at 16 (its 5 cells inside a window, 3 eighths)

static inline void rune_glyph(uint seed, byte glyph[rune_size][rune_size]) {
    memset(glyph, 0, rune_size * rune_size);
    for (byte r = 0; r < rune_size; r++) {
        glyph[r][2] = 1;
        seed = seed_rand(seed + r);
        const byte arm = seed % 4;
        if (arm == 1) {
            glyph[r][0] = glyph[r][1] = 1;
        } else if (arm == 2) {
            glyph[r][1] = 1;
        } else if (arm == 3 && r + 1 < rune_size) {
            glyph[r][1] = 1;
            glyph[r + 1][0] = 1;
        }
    }
    for (byte r = 0; r < rune_size; r++) {
        glyph[r][3] = glyph[r][1];
        glyph[r][4] = glyph[r][0];
    }
}

void model_rune_node_event(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity node = zox_get_link(world, e, CurrentNodeLink);
        if (!zox_valid(node) || !zox_has(node, NodeType) || zox_getv(node, NodeType) != zox_model_node_rune) {
            continue;
        }
        const entity model = zox_get_link(world, e, ModelLink);
        if (zox_valid(model) && zox_has(model, ModelActive) && zox_has(model, VoxelNode) && zox_has(model, NodeDepth)) {
            const byte depth = zox_getv(model, NodeDepth);
            const int length = octree_size(depth);
            const int cell = int_max(1, length * rune_cell / 16);
            const int start = length / 2 - rune_size * cell / 2;
            const byte voxel = zox_getv(node, NodeVoxel);
            byte glyph[rune_size][rune_size];
            rune_glyph(seed_rand((uint) zox_getv(model, Seed) + 9311), glyph);
            spinlock* lock = zox_octree_lock(world, model);
            zox_octree_lock_begin(lock);
            VoxelNode* voxels = zox_getm(model, VoxelNode);
            for (byte face = 0; face < 6; face++) {
                const byte axis = face / 2;
                const int sign = face % 2 ? -1 : 1;     // inward
                const byte a = axis == 0 ? 1 : 0, b = axis == 2 ? 1 : 2;
                for (int u = 0; u < rune_size * cell; u++) for (int v = 0; v < rune_size * cell; v++) {
                    // row from the top (b's high end down), column along a
                    if (!glyph[rune_size - 1 - v / cell][u / cell]) {
                        continue;
                    }
                    int3 p;
                    *int3_axis(&p, a) = start + u;
                    *int3_axis(&p, b) = start + v;
                    *int3_axis(&p, axis) = sign > 0 ? 0 : length - 1;
                    for (int d = 0; d < length; d++) {
                        const byte3 q = int3_to_byte3(p);
                        if (getv_VoxelNode(voxels, depth, q)) {
                            set_VoxelNode(voxels, depth, q, voxel);
                            break;
                        }
                        *int3_axis(&p, axis) += sign;
                    }
                }
            }
            zox_octree_lock_end(lock);
        }
        zox_add(e, TriggerEnd);
    }
}
