// tst: chunk_sides, faces between chunks are culled when the neighbor chunk is solid there
//  - builds a chunk's SidesOctree like chunk_sides_system (build_sides_dig), no window or rendering
//  - a neighbor chunk on each of the 6 sides, faces on the border are checked at every render depth,
//    as lod chunks draw their sides at a lower depth than their voxels
//  - the neighbor's face that touches the chunk decides it, never its far face
//  - cases:
//      - touching: neighbor solid where it touches, air on its far face -> no border faces
//      - hole: one air voxel where the neighbor touches -> one border face
//      - solid / air neighbor -> none / all border faces
//      - lod seams: neighbor with more detail (deeper tree), and with less detail
//      - inside: an air voxel in a chunk walled by solid neighbors -> exactly its 6 faces
//  - run: bash bsh/tst.sh chunk_sides

#define zox_tst_sides_depth 3   // 8x8x8 chunks, small and fast

// what the neighbor looks like, in the direction it sits from the chunk
#define zox_tst_sides_solid 0
#define zox_tst_sides_air 1
#define zox_tst_sides_far_air 2      // air on the face away from the chunk
#define zox_tst_sides_touch_hole 3   // one air voxel on the face touching the chunk

static inline byte zox_tst_sides_is_air(byte shape, byte direction, byte3 p, int length) {
    const byte axis = direction >> 1;
    const byte positive = direction & 1;
    const int coordinate = axis == 0 ? p.x : (axis == 1 ? p.y : p.z);
    // the neighbor sits on the positive side: it touches the chunk with its 0 face
    const int touching = positive ? 0 : length - 1;
    const int far = positive ? length - 1 : 0;
    switch (shape) {
        case zox_tst_sides_air:
            return 1;
        case zox_tst_sides_far_air:
            return coordinate == far;
        case zox_tst_sides_touch_hole: {
            // the voxel in the middle of the touching face
            const int middle = length / 2;
            const int a = axis == 0 ? p.y : p.x;
            const int b = axis == 2 ? p.y : p.z;
            return coordinate == touching && a == middle && b == middle;
        }
        default:
            return 0;
    }
}

static inline void zox_tst_sides_fill(VoxelNode* node, byte depth, byte shape, byte direction) {
    create_VoxelNode(node);
    const int length = octree_size(depth);
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        const byte3 p = (byte3) { x, y, z };
        set_VoxelNode(node, depth, p, zox_tst_sides_is_air(shape, direction, p, length) ? 0 : 1);
    }
    // generated chunks are reduced, so solid areas are big nodes
    reduce_VoxelNode(node);
}

// sides of chunk, with neighbors[6] (NULL for none), like chunk_sides_system, all blocks solid
static inline void zox_tst_sides_build(
    SidesOctree* sides,
    const VoxelNode* chunk,
    byte chunk_depth,
    const VoxelNode** neighbors,
    const byte* neighbor_depths)
{
    init_side_child_indices();
    create_SidesOctree(sides);
    build_sides_dig(NULL, chunk, neighbors, neighbor_depths, chunk, sides, chunk_depth, 0, byte3_zero, 0);
}

// faces pointing in direction, on the chunk's border in that direction, at render depth
static inline int zox_tst_sides_border_faces(const SidesOctree* sides, byte depth, byte direction) {
    const int length = octree_size(depth);
    const byte axis = direction >> 1;
    const int edge = (direction & 1) ? length - 1 : 0;
    int faces = 0;
    for (int a = 0; a < length; a++) for (int b = 0; b < length; b++) {
        byte3 p;
        if (axis == 0) p = (byte3) { edge, a, b };
        else if (axis == 1) p = (byte3) { a, edge, b };
        else p = (byte3) { a, b, edge };
        const SidesOctree* node = get_SidesOctree(sides, depth, p);
        faces += node && (node->value & (1 << (direction + 1)));
    }
    return faces;
}

// all faces in the chunk, any direction, at render depth
static inline int zox_tst_sides_all_faces(const SidesOctree* sides, byte depth) {
    const int length = octree_size(depth);
    int faces = 0;
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        const SidesOctree* node = get_SidesOctree(sides, depth, (byte3) { x, y, z });
        if (!node) {
            continue;
        }
        for (byte d = 0; d < 6; d++) {
            faces += (node->value >> (d + 1)) & 1;
        }
    }
    return faces;
}

static const char* zox_tst_sides_direction_names[6] = { "left", "right", "down", "up", "back", "front" };
static const char* zox_tst_sides_shape_names[4] = { "solid", "air", "far air", "touch hole" };

// a solid chunk with one neighbor, checks its border faces toward it at every render depth
//  - expected faces at a depth: -1 = every border node, else that count
static inline void zox_tst_sides_case(
    byte shape,
    byte chunk_depth,
    byte neighbor_depth,
    int expected_faces)
{
    for (byte direction = 0; direction < 6; direction++) {
        VoxelNode chunk;
        zox_tst_sides_fill(&chunk, chunk_depth, zox_tst_sides_solid, direction);
        VoxelNode neighbor;
        zox_tst_sides_fill(&neighbor, neighbor_depth, shape, direction);
        const VoxelNode* neighbors[6] = { 0 };
        byte neighbor_depths[6] = { 0 };
        neighbors[direction] = &neighbor;
        neighbor_depths[direction] = neighbor_depth;
        SidesOctree sides;
        zox_tst_sides_build(&sides, &chunk, chunk_depth, neighbors, neighbor_depths);
        for (byte depth = 0; depth <= chunk_depth; depth++) {
            const int length = octree_size(depth);
            int expected = expected_faces < 0 ? length * length : expected_faces;
            // a hole smaller than the chunk's node faces the whole node, so still one face
            const int faces = zox_tst_sides_border_faces(&sides, depth, direction);
            zox_tst_check(faces == expected,
                "[%s] neighbor [%s] chunk depth [%i] neighbor depth [%i] render depth [%i]: [%i] border faces, expected [%i]",
                zox_tst_sides_direction_names[direction], zox_tst_sides_shape_names[shape],
                chunk_depth, neighbor_depth, depth, faces, expected);
        }
        collapse_SidesOctree(&sides);
        collapse_VoxelNode(&neighbor);
        collapse_VoxelNode(&chunk);
    }
}

// an air voxel inside a chunk walled in by solid neighbors: only its 6 faces exist
static inline void zox_tst_sides_inside() {
    const byte depth = zox_tst_sides_depth;
    VoxelNode chunk;
    zox_tst_sides_fill(&chunk, depth, zox_tst_sides_solid, 0);
    const int middle = octree_size(depth) / 2;
    set_VoxelNode(&chunk, depth, (byte3) { middle, middle, middle }, 0);
    VoxelNode solid;
    zox_tst_sides_fill(&solid, depth, zox_tst_sides_solid, 0);
    const VoxelNode* neighbors[6];
    byte neighbor_depths[6];
    for (byte d = 0; d < 6; d++) {
        neighbors[d] = &solid;
        neighbor_depths[d] = depth;
    }
    SidesOctree sides;
    zox_tst_sides_build(&sides, &chunk, depth, neighbors, neighbor_depths);
    const int faces = zox_tst_sides_all_faces(&sides, depth);
    zox_tst_check(faces == 6, "inside: [%i] faces around one air voxel in a walled chunk, expected [6]", faces);
    collapse_SidesOctree(&sides);
    collapse_VoxelNode(&solid);
    collapse_VoxelNode(&chunk);
}

byte zox_tst_chunk_sides_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_chunk_sides_check(ecs* world) {
    (void) world;
    const byte depth = zox_tst_sides_depth;
    // same detail on both sides
    zox_tst_sides_case(zox_tst_sides_solid, depth, depth, 0);
    zox_tst_sides_case(zox_tst_sides_air, depth, depth, -1);
    zox_tst_sides_case(zox_tst_sides_far_air, depth, depth, 0);
    zox_tst_sides_case(zox_tst_sides_touch_hole, depth, depth, 1);
    // lod seam: the neighbor has more detail than the chunk
    zox_tst_sides_case(zox_tst_sides_far_air, depth - 1, depth, 0);
    zox_tst_sides_case(zox_tst_sides_touch_hole, depth - 1, depth, 1);
    // lod seam: the neighbor has less detail, its one air voxel covers 2x2 of the chunk's
    zox_tst_sides_case(zox_tst_sides_far_air, depth, depth - 1, 0);
    zox_tst_sides_inside();
}

void zox_tsts_chunks3_sides() {
    zox_tst_add("chunk_sides", zox_tst_chunk_sides_ready, zox_tst_chunk_sides_check, 30);
}
