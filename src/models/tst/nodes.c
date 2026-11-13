// tst: model_nodes, what each model node does to voxels (src/models/fun: the functions the node systems call)
//  - fill (ellipsoid): inside its box, symmetric about its centre, round (no lone voxel at a pole)
//  - box: exactly its box, voxel 0 cuts
//  - paint: exactly its box, only voxels already solid, never adds or removes
//  - sprinkle (raised): dots in air on the surface; speckle: spots in solid on the surface; both:
//      the count asked, the same for a seed, different for another, dots snapped to their size, and a count
//      over the places fills them all
//  - pure logic on the main menu; run: bash bsh/tst.sh model_nodes
#define zox_tst_nodes_depth 4   // 16 voxels a side

typedef struct {
    int solid;
    int3 lower;
    int3 upper;
} zox_tst_voxels;

zox_tst_voxels zox_tst_nodes_scan(VoxelNode* tree, byte value) {
    const int length = octree_size(zox_tst_nodes_depth);
    zox_tst_voxels v = { 0, { length, length, length }, { -1, -1, -1 } };
    for (int x = 0; x < length; x++)
    for (int y = 0; y < length; y++)
    for (int z = 0; z < length; z++) {
        const byte voxel = getv_VoxelNode(tree, zox_tst_nodes_depth, (byte3) { x, y, z });
        if (value ? voxel == value : voxel != 0) {
            v.solid++;
            v.lower = (int3) { int_min(v.lower.x, x), int_min(v.lower.y, y), int_min(v.lower.z, z) };
            v.upper = (int3) { int_max(v.upper.x, x), int_max(v.upper.y, y), int_max(v.upper.z, z) };
        }
    }
    return v;
}

static inline byte zox_tst_nodes_get(VoxelNode* tree, int x, int y, int z) {
    const int length = octree_size(zox_tst_nodes_depth);
    if (x < 0 || y < 0 || z < 0 || x >= length || y >= length || z >= length) {
        return 0;
    }
    return getv_VoxelNode(tree, zox_tst_nodes_depth, (byte3) { x, y, z });
}

// voxels that differ between two trees
int zox_tst_nodes_differences(VoxelNode* a, VoxelNode* b) {
    const int length = octree_size(zox_tst_nodes_depth);
    int differences = 0;
    for (int x = 0; x < length; x++)
    for (int y = 0; y < length; y++)
    for (int z = 0; z < length; z++) {
        differences += zox_tst_nodes_get(a, x, y, z) != zox_tst_nodes_get(b, x, y, z);
    }
    return differences;
}

// a solid ball to sprinkle on
void zox_tst_nodes_ball(VoxelNode* tree) {
    create_VoxelNode(tree);
    voctree_fill_ellipsoid(tree, zox_tst_nodes_depth, 1, byte3_single(8), byte3_single(10));
}

byte zox_tst_model_nodes_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_model_nodes_ellipsoid(void) {
    VoxelNode tree;
    create_VoxelNode(&tree);
    const byte3 centre = { 8, 7, 8 };
    const byte3 size = { 10, 6, 8 };   // radii 5, 3, 4
    voctree_fill_ellipsoid(&tree, zox_tst_nodes_depth, 1, centre, size);
    const zox_tst_voxels v = zox_tst_nodes_scan(&tree, 0);
    zox_tst_check(v.solid > 0, "the ellipsoid filled nothing");
    zox_tst_check(v.lower.x == 3 && v.upper.x == 13 && v.lower.y == 4 && v.upper.y == 10 && v.lower.z == 4 && v.upper.z == 12,
        "the ellipsoid spans [%i-%i %i-%i %i-%i], its box is [3-13 4-10 4-12]",
        v.lower.x, v.upper.x, v.lower.y, v.upper.y, v.lower.z, v.upper.z);
    int asymmetric = 0;
    for (int dx = 0; dx <= 6; dx++)
    for (int dy = 0; dy <= 6; dy++)
    for (int dz = 0; dz <= 6; dz++) {
        const byte a = zox_tst_nodes_get(&tree, centre.x + dx, centre.y + dy, centre.z + dz);
        asymmetric += a != zox_tst_nodes_get(&tree, centre.x - dx, centre.y + dy, centre.z + dz);
        asymmetric += a != zox_tst_nodes_get(&tree, centre.x + dx, centre.y - dy, centre.z + dz);
        asymmetric += a != zox_tst_nodes_get(&tree, centre.x + dx, centre.y + dy, centre.z - dz);
    }
    zox_tst_check(!asymmetric, "the ellipsoid isn't symmetric about its centre, [%i] mirrored voxels differ", asymmetric);
    // round: the widest pole (x, radius 5) is a cap of voxels, not one sticking out
    int pole = 0;
    for (int y = 0; y < 16; y++)
    for (int z = 0; z < 16; z++) {
        pole += zox_tst_nodes_get(&tree, centre.x + 5, y, z) != 0;
    }
    zox_tst_check(pole > 1, "the ellipsoid's pole is a lone voxel ([%i]), it pokes out", pole);
    close_VoxelNode(&tree);
}

void zox_tst_model_nodes_box_and_paint(void) {
    VoxelNode tree;
    create_VoxelNode(&tree);
    voctree_fill_cube(&tree, zox_tst_nodes_depth, 1, (byte3) { 2, 3, 4 }, (byte3) { 5, 4, 3 });
    zox_tst_voxels v = zox_tst_nodes_scan(&tree, 0);
    zox_tst_check(v.solid == 60 && v.lower.x == 2 && v.upper.x == 6 && v.lower.y == 3 && v.upper.y == 6 && v.lower.z == 4 && v.upper.z == 6,
        "a 5x4x3 box at 2x3x4 filled [%i] voxels over [%i-%i %i-%i %i-%i]",
        v.solid, v.lower.x, v.upper.x, v.lower.y, v.upper.y, v.lower.z, v.upper.z);
    // cut a 1 thick slice with air
    voctree_fill_cube(&tree, zox_tst_nodes_depth, 0, (byte3) { 2, 3, 4 }, (byte3) { 5, 4, 1 });
    v = zox_tst_nodes_scan(&tree, 0);
    zox_tst_check(v.solid == 40, "cutting a 5x4x1 slice left [%i] voxels, expected [40]", v.solid);
    // paint a box half over the solid, half over air: its size, only what's solid
    voctree_paint(&tree, zox_tst_nodes_depth, 2, (byte3) { 4, 3, 4 }, (byte3) { 2, 2, 2 });
    const zox_tst_voxels painted = zox_tst_nodes_scan(&tree, 2);
    const zox_tst_voxels after = zox_tst_nodes_scan(&tree, 0);
    // the box 4-5 x 3-4 y 4-5 z, z 4 was cut: 2x2x1 solid in it
    zox_tst_check(painted.solid == 4, "a 2x2x2 paint over 2x2x1 of solid painted [%i], expected [4]", painted.solid);
    zox_tst_check(painted.solid == 0 || (painted.upper.x <= 5 && painted.upper.y <= 4 && painted.upper.z <= 5),
        "paint went past its 2x2x2 box, to [%ix%ix%i]", painted.upper.x, painted.upper.y, painted.upper.z);
    zox_tst_check(after.solid == 40, "painting changed the solid voxels [40] to [%i]", after.solid);
    close_VoxelNode(&tree);
}

void zox_tst_model_nodes_sprinkle(void) {
    const int length = octree_size(zox_tst_nodes_depth);
    const byte3 everywhere = byte3_zero;
    const byte3 all = byte3_single(length);
    for (byte raised = 0; raised <= 1; raised++) {
        const char* name = raised ? "sprinkle" : "speckle";
        VoxelNode a, b, c;
        zox_tst_nodes_ball(&a);
        zox_tst_nodes_ball(&b);
        zox_tst_nodes_ball(&c);
        const int before = zox_tst_nodes_scan(&a, 0).solid;
        const int set_a = voctree_sprinkle(&a, zox_tst_nodes_depth, 2, everywhere, all, 12, 100, raised, 1);
        voctree_sprinkle(&b, zox_tst_nodes_depth, 2, everywhere, all, 12, 100, raised, 1);
        voctree_sprinkle(&c, zox_tst_nodes_depth, 2, everywhere, all, 12, 200, raised, 1);
        zox_tst_check(set_a == 12, "%s set [%i] dots, asked for [12]", name, set_a);
        zox_tst_check(zox_tst_nodes_differences(&a, &b) == 0, "%s differs for the same seed", name);
        zox_tst_check(zox_tst_nodes_differences(&a, &c) > 0, "%s is the same for another seed", name);
        const zox_tst_voxels dots = zox_tst_nodes_scan(&a, 2);
        const int after = zox_tst_nodes_scan(&a, 0).solid;
        zox_tst_check(dots.solid == 12, "%s made [%i] dot voxels, expected [12]", name, dots.solid);
        zox_tst_check(after == before + (raised ? 12 : 0), "%s: solid [%i] to [%i], %s", name, before, after,
            raised ? "dots add to it" : "spots don't add");
        // each dot on the surface: raised ones touch the ball (1), speckles touch air
        int off_surface = 0;
        for (int x = 0; x < length; x++)
        for (int y = 0; y < length; y++)
        for (int z = 0; z < length; z++) {
            if (zox_tst_nodes_get(&a, x, y, z) != 2) {
                continue;
            }
            const int3 sides[6] = { { 1, 0, 0 }, { -1, 0, 0 }, { 0, 1, 0 }, { 0, -1, 0 }, { 0, 0, 1 }, { 0, 0, -1 } };
            byte touches = 0;
            for (int s = 0; s < 6; s++) {
                const byte n = zox_tst_nodes_get(&a, x + sides[s].x, y + sides[s].y, z + sides[s].z);
                touches |= raised ? n == 1 : n == 0;
            }
            off_surface += !touches;
        }
        zox_tst_check(!off_surface, "[%i] %s dots aren't on the surface", off_surface, name);
        close_VoxelNode(&a);
        close_VoxelNode(&b);
        close_VoxelNode(&c);
    }
    // dots of 2: whole 2x2x2 cells on the grid
    VoxelNode d;
    zox_tst_nodes_ball(&d);
    voctree_sprinkle(&d, zox_tst_nodes_depth, 2, everywhere, all, 6, 7, 0, 2);
    int unaligned = 0;
    for (int x = 0; x < length; x++)
    for (int y = 0; y < length; y++)
    for (int z = 0; z < length; z++) {
        if (zox_tst_nodes_get(&d, x, y, z) != 2) {
            continue;
        }
        // its cell's solid voxels all painted
        const int3 cell = { x - x % 2, y - y % 2, z - z % 2 };
        for (int dx = 0; dx < 2; dx++)
        for (int dy = 0; dy < 2; dy++)
        for (int dz = 0; dz < 2; dz++) {
            const byte n = zox_tst_nodes_get(&d, cell.x + dx, cell.y + dy, cell.z + dz);
            unaligned += n == 1;
        }
    }
    zox_tst_check(!unaligned, "size 2 speckles left [%i] solid voxels unpainted in their cells", unaligned);
    close_VoxelNode(&d);
    // more dots than places: every place, within the box
    VoxelNode e;
    zox_tst_nodes_ball(&e);
    const int filled = voctree_sprinkle(&e, zox_tst_nodes_depth, 2, (byte3) { 0, 0, 0 }, (byte3) { 4, 16, 16 }, 250, 3, 1, 1);
    const zox_tst_voxels in_box = zox_tst_nodes_scan(&e, 2);
    zox_tst_check(filled > 0 && filled < 250 && in_box.solid == filled, "250 sprinkles in a small box set [%i], [%i] voxels", filled, in_box.solid);
    zox_tst_check(in_box.solid == 0 || in_box.upper.x < 4, "sprinkles past their box, to x [%i]", in_box.upper.x);
    close_VoxelNode(&e);
}

void zox_tst_model_nodes_check(ecs* world) {
    (void) world;
    zox_tst_model_nodes_ellipsoid();
    zox_tst_model_nodes_box_and_paint();
    zox_tst_model_nodes_sprinkle();
}
