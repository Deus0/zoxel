// tst: octrees, pure logic tests for src/_/octrees (no world, no terrain, no gpu)
//  - checked against a dense byte array reference, so any walk/set/reduce bug shows as a mismatch
//  - run: bash bsh/tst.sh octree_set_get (or any name below), bash bsh/tst.sh all
//  - included from src/core/tst/_.c, as "_" is below the tst harness
//  - every test is ready on the first frame, they never wait for the game

// same layout as VoxelNode: children ptr first, value after it, then type
typedef struct {
    void* ptr;
    byte value;
    byte type;
} TstNode;

// same layout as LightNode / SidesOctree: no type
typedef struct {
    void* ptr;
    byte value;
} TstBasicNode;

zox_octree_functions(TstNode, 0, offsetof(TstNode, type));
create_octree_setters(TstNode);
create_octree_accessors(TstNode);
create_octree_reducer(TstNode);
create_octree_optimizer(TstNode, 1);
create_octree_queries(TstNode);
create_node_neighbor(TstNode);
zox_node_add_link(TstNode);
zox_octree_functions(TstBasicNode, 0, 0);
create_octree_setters(TstBasicNode);
create_octree_accessors(TstBasicNode);
create_octree_reducer(TstBasicNode);
create_octree_optimizer(TstBasicNode, 0);
create_optimize_by_max(TstBasicNode);
create_octree_get_nearby(TstBasicNode);

#define zox_tst_octree_depth 5
#define zox_tst_octree_size (1 << zox_tst_octree_depth)
#define zox_tst_octree_volume (zox_tst_octree_size * zox_tst_octree_size * zox_tst_octree_size)

// deterministic, so a failure reproduces
uint zox_tst_octree_seed = 0;

static inline uint zox_tst_octree_rand() {
    uint x = zox_tst_octree_seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    zox_tst_octree_seed = x;
    return x;
}

static inline int zox_tst_octree_index(byte3 p) {
    return p.x + p.y * zox_tst_octree_size + p.z * zox_tst_octree_size * zox_tst_octree_size;
}

static inline byte3 zox_tst_octree_position(int i) {
    return (byte3) {
        i % zox_tst_octree_size,
        (i / zox_tst_octree_size) % zox_tst_octree_size,
        i / (zox_tst_octree_size * zox_tst_octree_size)
    };
}

static inline byte3 zox_tst_octree_random_position() {
    return zox_tst_octree_position(zox_tst_octree_rand() % zox_tst_octree_volume);
}

static inline void zox_tst_octree_init(TstNode* node, byte value) {
    octree_create(node, sizeof(TstNode), value);
}

static inline void zox_tst_octree_free(TstNode* node) {
    collapse_TstNode(node);
}

// fills a tree and the reference with the same random voxels, values in [0, values)
static inline void zox_tst_octree_fill_random(
    TstNode* root,
    byte* reference,
    int count,
    byte values)
{
    zox_tst_octree_init(root, 0);
    memset(reference, 0, zox_tst_octree_volume);
    for (int i = 0; i < count; i++) {
        byte3 p = zox_tst_octree_random_position();
        byte value = zox_tst_octree_rand() % values;
        set_TstNode(root, zox_tst_octree_depth, p, value);
        reference[zox_tst_octree_index(p)] = value;
    }
}

// one check per call (not per voxel) so a broken tree doesn't spam the log
static inline void zox_tst_octree_compare(
    const TstNode* root,
    const byte* reference,
    const char* label)
{
    int mismatches = 0;
    int first = -1;
    for (int i = 0; i < zox_tst_octree_volume; i++) {
        byte3 p = zox_tst_octree_position(i);
        if (getv_TstNode(root, zox_tst_octree_depth, p) != reference[i]) {
            if (first == -1) {
                first = i;
            }
            mismatches++;
        }
    }
    byte3 p = zox_tst_octree_position(first == -1 ? 0 : first);
    zox_tst_check(!mismatches,
        "[%s] [%i] voxels differ from reference, first at [%ix%ix%i] tree [%i] reference [%i]",
        label,
        mismatches,
        p.x, p.y, p.z,
        getv_TstNode(root, zox_tst_octree_depth, p),
        reference[zox_tst_octree_index(p)]);
}

static inline int zox_tst_octree_count_nodes(const TstNode* node) {
    if (!node->ptr) {
        return 1;
    }
    const TstNode* kids = (const TstNode*) node->ptr;
    int count = 1;
    for (int i = 0; i < octree_length; i++) {
        count += zox_tst_octree_count_nodes(&kids[i]);
    }
    return count;
}

// open nodes must have all 8 children uniform-free after a reduce
static inline int zox_tst_octree_count_reducible(const TstNode* node) {
    if (!node->ptr) {
        return 0;
    }
    const TstNode* kids = (const TstNode*) node->ptr;
    int count = 0;
    byte uniform = 1;
    for (int i = 0; i < octree_length; i++) {
        count += zox_tst_octree_count_reducible(&kids[i]);
        if (kids[i].ptr || kids[i].value != kids[0].value) {
            uniform = 0;
        }
    }
    return count + uniform;
}

byte zox_tst_octrees_ready(ecs* world) {
    (void) world;
    return 1;
}

// --- octree_basics: index maths, sizes, sides, create, depth 0, out of bounds ---
void zox_tst_octree_basics_check(ecs* world) {
    (void) world;
    for (byte i = 0; i < octree_length; i++) {
        zox_tst_check(byte3_octree_array_index(octree_positions[i]) == i,
            "octree_positions[%i] maps to index [%i]", i, byte3_octree_array_index(octree_positions[i]));
    }
    for (byte d = 0; d <= 8; d++) {
        zox_tst_check(octree_size(d) == (1u << d), "octree_size(%i) is [%i]", d, octree_size(d));
    }
    init_side_child_indices();
    for (byte axis = 0; axis < 3; axis++) {
        for (byte side = 0; side < 2; side++) {
            byte seen = 0;
            for (byte j = 0; j < 4; j++) {
                byte i = side_child_indices[axis][side][j];
                zox_tst_check(((i >> axis) & 1) == side,
                    "side child [%i] is not on axis [%i] side [%i]", i, axis, side);
                seen |= 1 << i;
            }
            byte other = 0;
            for (byte j = 0; j < 4; j++) {
                other |= 1 << side_child_indices[axis][!side][j];
            }
            zox_tst_check((seen | other) == 0xff && !(seen & other),
                "axis [%i] sides don't split the 8 children", axis);
        }
    }
    for (byte dir = 0; dir < 6; dir++) {
        zox_tst_check(side_infos[side_infos[dir].opposite].opposite == dir,
            "side_infos [%i] opposite isn't symmetric", dir);
        zox_tst_check(reverse_direction(dir) == side_infos[dir].opposite,
            "side_infos [%i] opposite [%i] != reverse_direction [%i]", dir, side_infos[dir].opposite, reverse_direction(dir));
    }
    TstNode root;
    zox_tst_octree_init(&root, 7);
    zox_tst_check(!root.ptr && root.value == 7 && !root.type, "create didn't give a closed node of value 7");
    // a closed tree reads its root value everywhere
    zox_tst_check(getv_TstNode(&root, zox_tst_octree_depth, (byte3) { 31, 0, 17 }) == 7, "closed tree didn't read root value");
    zox_tst_check(get_TstNode(&root, 0, byte3_zero) == &root, "depth 0 get didn't return the root");
    zox_tst_check(get_TstNode(&root, zox_tst_octree_depth, (byte3) { 3, 3, 3 }) == &root, "get on a closed tree didn't stop at the root");
    // out of bounds never reads, writes or allocates
    byte3 oob[] = { { 32, 0, 0 }, { 0, 32, 0 }, { 0, 0, 32 }, { 255, 255, 255 } };
    for (byte i = 0; i < 4; i++) {
        zox_tst_check(!get_TstNode(&root, zox_tst_octree_depth, oob[i]), "oob get [%i] returned a node", i);
        zox_tst_check(!getv_TstNode(&root, zox_tst_octree_depth, oob[i]), "oob getv [%i] returned a value", i);
        zox_tst_check(!set_TstNode(&root, zox_tst_octree_depth, oob[i], 3), "oob set [%i] returned a node", i);
    }
    zox_tst_check(!root.ptr, "oob calls opened the root");
    zox_tst_check(!get_TstNode(NULL, 3, byte3_zero), "get on NULL returned a node");
    zox_tst_check(!getv_TstNode(NULL, 3, byte3_zero), "getv on NULL returned a value");
    // open_one makes 8 children that inherit
    open_one_TstNode(&root);
    zox_tst_check(root.ptr != NULL, "open_one didn't open the root");
    if (root.ptr) {
        TstNode* kids = (TstNode*) root.ptr;
        for (byte i = 0; i < octree_length; i++) {
            zox_tst_check(kids[i].value == 7 && !kids[i].ptr, "open_one child [%i] value [%i]", i, kids[i].value);
        }
    }
    zox_tst_octree_free(&root);
    zox_tst_check(!root.ptr && root.value == 7, "collapse didn't close the root, or lost its value");
}

// --- octree_set_get: single sets, inherit vs clean, random fill against a dense reference ---
void zox_tst_octree_set_get_check(ecs* world) {
    (void) world;
    TstNode root;
    // inherit: siblings keep the parent's value
    zox_tst_octree_init(&root, 4);
    byte3 p = { 5, 9, 30 };
    TstNode* leaf = set_TstNode(&root, zox_tst_octree_depth, p, 2);
    zox_tst_check(leaf != NULL && leaf->value == 2, "set didn't return the leaf");
    zox_tst_check(get_TstNode(&root, zox_tst_octree_depth, p) == leaf, "get didn't find the set leaf");
    zox_tst_check(getm_TstNode(&root, p, zox_tst_octree_depth) == leaf, "getm didn't find the set leaf");
    zox_tst_check(getv_TstNode(&root, zox_tst_octree_depth, p) == 2, "getv after set");
    zox_tst_check(getv_TstNode(&root, zox_tst_octree_depth, (byte3) { 4, 9, 30 }) == 4, "inherited sibling lost the parent value");
    zox_tst_check(getv_TstNode(&root, zox_tst_octree_depth, (byte3) { 0, 0, 0 }) == 4, "far voxel lost the root value");
    zox_tst_check(zox_tst_octree_count_nodes(&root) == 1 + 8 * zox_tst_octree_depth,
        "one set opened [%i] nodes, expected [%i]", zox_tst_octree_count_nodes(&root), 1 + 8 * zox_tst_octree_depth);
    // shallower reads stop at the open node above
    zox_tst_check(get_TstNode(&root, 1, (byte3) { 0, 1, 1 }) == &((TstNode*) root.ptr)[6], "depth 1 get went to the wrong child");
    zox_tst_octree_free(&root);
    // clean: siblings are 0
    zox_tst_octree_init(&root, 4);
    zox_tst_check(set_clean_TstNode(&root, zox_tst_octree_depth, p, 2), "set_clean failed");
    zox_tst_check(getv_TstNode(&root, zox_tst_octree_depth, p) == 2, "getv after set_clean");
    zox_tst_check(getv_TstNode(&root, zox_tst_octree_depth, (byte3) { 4, 9, 30 }) == 0, "set_clean sibling isn't 0");
    zox_tst_octree_free(&root);
    // open: opens the path without changing values
    zox_tst_octree_init(&root, 9);
    TstNode* opened = open_TstNode(&root, 3, (byte3) { 7, 0, 7 });
    zox_tst_check(opened && opened->value == 9 && !opened->ptr, "open didn't give a closed leaf with the parent value");
    zox_tst_check(get_TstNode(&root, 3, (byte3) { 7, 0, 7 }) == opened, "open leaf isn't where get finds it");
    zox_tst_octree_free(&root);
    // every corner and the middle
    zox_tst_octree_init(&root, 0);
    const byte m = zox_tst_octree_size - 1;
    byte3 corners[] = { {0,0,0}, {m,0,0}, {0,m,0}, {m,m,0}, {0,0,m}, {m,0,m}, {0,m,m}, {m,m,m}, {16,16,16}, {15,15,15} };
    for (byte i = 0; i < 10; i++) {
        set_TstNode(&root, zox_tst_octree_depth, corners[i], i + 1);
    }
    for (byte i = 0; i < 10; i++) {
        zox_tst_check(getv_TstNode(&root, zox_tst_octree_depth, corners[i]) == i + 1,
            "corner [%ix%ix%i] reads [%i]", corners[i].x, corners[i].y, corners[i].z,
            getv_TstNode(&root, zox_tst_octree_depth, corners[i]));
    }
    zox_tst_octree_free(&root);
    // random fill, sparse and dense
    static byte reference[zox_tst_octree_volume];
    int fills[] = { 50, 2000, 40000 };
    for (byte f = 0; f < 3; f++) {
        zox_tst_octree_seed = 1234 + f;
        zox_tst_octree_fill_random(&root, reference, fills[f], 6);
        zox_tst_octree_compare(&root, reference, fills[f] == 50 ? "sparse" : fills[f] == 2000 ? "medium" : "dense");
        // overwrite everything with one value, tree reads it everywhere
        for (int i = 0; i < zox_tst_octree_volume; i++) {
            set_TstNode(&root, zox_tst_octree_depth, zox_tst_octree_position(i), 3);
        }
        memset(reference, 3, zox_tst_octree_volume);
        zox_tst_octree_compare(&root, reference, "overwrite");
        zox_tst_octree_free(&root);
    }
}

// --- octree_reduce: reduce keeps every voxel, collapses what it can, respects linked nodes ---
void zox_tst_octree_reduce_check(ecs* world) {
    (void) world;
    static byte reference[zox_tst_octree_volume];
    TstNode root;
    int fills[] = { 30, 3000, 40000 };
    for (byte f = 0; f < 3; f++) {
        zox_tst_octree_seed = 99 + f;
        // few values, so plenty of uniform branches form
        zox_tst_octree_fill_random(&root, reference, fills[f], 2);
        int before = zox_tst_octree_count_nodes(&root);
        reduce_TstNode(&root);
        int after = zox_tst_octree_count_nodes(&root);
        zox_tst_octree_compare(&root, reference, "reduced");
        zox_tst_check(after <= before, "reduce grew the tree [%i] -> [%i]", before, after);
        zox_tst_check(!zox_tst_octree_count_reducible(&root),
            "reduce left [%i] uniform branches open", zox_tst_octree_count_reducible(&root));
        // reducing twice changes nothing
        reduce_TstNode(&root);
        zox_tst_check(zox_tst_octree_count_nodes(&root) == after, "second reduce changed the tree");
        zox_tst_octree_free(&root);
    }
    // a fully uniform tree collapses to its root
    zox_tst_octree_init(&root, 0);
    for (int i = 0; i < zox_tst_octree_volume; i++) {
        set_TstNode(&root, zox_tst_octree_depth, zox_tst_octree_position(i), 5);
    }
    zox_tst_check(reduce_TstNode(&root) == 1, "uniform reduce didn't report a collapse");
    zox_tst_check(!root.ptr && root.value == 5, "uniform tree didn't collapse to root value 5, value [%i]", root.value);
    zox_tst_octree_free(&root);
    // setting a voxel back to its region's value then reducing collapses the path again
    zox_tst_octree_init(&root, 2);
    byte3 p = { 17, 3, 29 };
    set_TstNode(&root, zox_tst_octree_depth, p, 8);
    set_TstNode(&root, zox_tst_octree_depth, p, 2);
    reduce_TstNode(&root);
    zox_tst_check(!root.ptr && root.value == 2, "set then unset didn't reduce back to a closed root");
    zox_tst_octree_free(&root);
    // linked (instance) nodes are never collapsed away
    zox_tst_octree_init(&root, 0);
    open_one_TstNode(&root);
    TstNode* kids = (TstNode*) root.ptr;
    OctreeLink* link = zox_malloc(sizeof(OctreeLink));
    *link = (OctreeLink) { 12345 };
    kids[3].ptr = link;
    kids[3].type = node_type_instance;
    zox_tst_check(!reduce_TstNode(&root), "reduce collapsed a branch with a linked node");
    zox_tst_check(root.ptr == kids && kids[3].ptr == link && kids[3].type == node_type_instance,
        "reduce changed the linked node");
    optimize_TstNode(&root);
    zox_tst_check(kids[3].ptr == link, "optimize changed the linked node");
    // collapse frees the children and the link (not treating the link as children)
    collapse_TstNode(&root);
    zox_tst_check(!root.ptr, "collapse didn't free the root's children");
}

// --- octree_optimize: parent values become the mode (or max) of their children, leaves untouched ---
void zox_tst_octree_optimize_check(ecs* world) {
    (void) world;
    // typed (voxels): air is skipped, so a mostly air node takes its solid value
    TstNode root;
    zox_tst_octree_init(&root, 0);
    open_one_TstNode(&root);
    byte typed_values[8] = { 0, 0, 0, 0, 0, 2, 3, 3 };
    for (byte i = 0; i < 8; i++) {
        ((TstNode*) root.ptr)[i].value = typed_values[i];
    }
    optimize_TstNode(&root);
    zox_tst_check(root.value == 3, "typed optimize picked [%i], expected solid mode [3]", root.value);
    for (byte i = 0; i < 8; i++) {
        zox_tst_check(((TstNode*) root.ptr)[i].value == typed_values[i], "optimize changed leaf [%i]", i);
    }
    // all air stays air
    for (byte i = 0; i < 8; i++) {
        ((TstNode*) root.ptr)[i].value = 0;
    }
    optimize_TstNode(&root);
    zox_tst_check(root.value == 0, "all air optimized to [%i]", root.value);
    zox_tst_octree_free(&root);
    // basic (lights): air counts, plain mode
    TstBasicNode basic;
    octree_create(&basic, sizeof(TstBasicNode), 0);
    open_one_TstBasicNode(&basic);
    byte basic_values[8] = { 0, 0, 0, 0, 0, 2, 3, 3 };
    for (byte i = 0; i < 8; i++) {
        ((TstBasicNode*) basic.ptr)[i].value = basic_values[i];
    }
    optimize_TstBasicNode(&basic);
    zox_tst_check(basic.value == 0, "basic optimize picked [%i], expected mode [0]", basic.value);
    optimize_by_max_TstBasicNode(&basic);
    zox_tst_check(basic.value == 3, "optimize_by_max picked [%i], expected [3]", basic.value);
    // deeper: max propagates bottom up
    TstBasicNode* deep = set_TstBasicNode(&basic, 4, (byte3) { 15, 15, 15 }, 200);
    zox_tst_check(deep != NULL, "basic deep set failed");
    optimize_by_max_TstBasicNode(&basic);
    zox_tst_check(basic.value == 200, "optimize_by_max didn't propagate a deep max, root [%i]", basic.value);
    zox_tst_check(getv_TstBasicNode(&basic, 4, (byte3) { 15, 15, 15 }) == 200, "optimize_by_max changed a leaf");
    zox_tst_check(reduce_TstBasicNode(&basic) == 0, "basic reduce collapsed a non uniform tree");
    collapse_TstBasicNode(&basic);
    // random: optimize never changes any leaf
    static byte reference[zox_tst_octree_volume];
    zox_tst_octree_seed = 777;
    zox_tst_octree_fill_random(&root, reference, 5000, 5);
    optimize_TstNode(&root);
    zox_tst_octree_compare(&root, reference, "optimized");
    zox_tst_octree_free(&root);
}

// --- octree_clone_move: deep copies, moves, collapse ---
void zox_tst_octree_clone_move_check(ecs* world) {
    (void) world;
    static byte reference[zox_tst_octree_volume];
    TstNode src;
    TstNode dst;
    zox_tst_octree_seed = 4242;
    zox_tst_octree_fill_random(&src, reference, 4000, 4);
    clone_TstNode(&dst, &src);
    zox_tst_octree_compare(&dst, reference, "clone");
    zox_tst_check(dst.ptr != src.ptr, "clone shares the children array");
    zox_tst_check(zox_tst_octree_count_nodes(&dst) == zox_tst_octree_count_nodes(&src), "clone node count differs");
    // writing to the source leaves the clone alone
    for (int i = 0; i < 2000; i++) {
        set_TstNode(&src, zox_tst_octree_depth, zox_tst_octree_random_position(), 9);
    }
    zox_tst_octree_compare(&dst, reference, "clone after source edits");
    zox_tst_octree_free(&src);
    zox_tst_octree_compare(&dst, reference, "clone after source freed");
    // move hands over the tree and resets the source
    TstNode moved;
    zox_tst_octree_init(&moved, 0);
    void* children = dst.ptr;
    octree_move(&moved, &dst, sizeof(TstNode), 6);
    zox_tst_check(moved.ptr == children, "move didn't hand over the children");
    zox_tst_check(!dst.ptr && dst.value == 6, "move didn't reset the source to the default value");
    zox_tst_octree_compare(&moved, reference, "moved");
    zox_tst_octree_free(&moved);
    // clone of a closed node
    zox_tst_octree_init(&src, 11);
    clone_TstNode(&dst, &src);
    zox_tst_check(!dst.ptr && dst.value == 11, "clone of a closed node");
    // collapse on a basic tree clears it, keeps the value
    TstBasicNode basic;
    octree_create(&basic, sizeof(TstBasicNode), 1);
    set_TstBasicNode(&basic, 3, (byte3) { 1, 2, 3 }, 5);
    collapse_TstBasicNode(&basic);
    zox_tst_check(!basic.ptr && basic.value == 1, "collapse didn't clear children or lost the value");
}

// --- octree_queries: max value, contains, bounds ---
void zox_tst_octree_queries_check(ecs* world) {
    (void) world;
    static byte reference[zox_tst_octree_volume];
    TstNode root;
    for (byte f = 0; f < 4; f++) {
        zox_tst_octree_seed = 31 + f;
        // mostly air, a few solids, so the bounds are interesting
        zox_tst_octree_init(&root, 0);
        memset(reference, 0, zox_tst_octree_volume);
        int count = f == 0 ? 0 : 1 + (zox_tst_octree_rand() % 40);
        for (int i = 0; i < count; i++) {
            byte3 p = zox_tst_octree_random_position();
            // keep some inside a smaller box
            if (f == 2) {
                p.x %= 12; p.y %= 5; p.z %= 20;
            }
            byte value = 1 + zox_tst_octree_rand() % 9;
            set_TstNode(&root, zox_tst_octree_depth, p, value);
            reference[zox_tst_octree_index(p)] = value;
        }
        if (f == 3) {
            reduce_TstNode(&root);
        }
        byte max = 0;
        byte has[256] = { 0 };
        byte3 bounds = byte3_zero;
        for (int i = 0; i < zox_tst_octree_volume; i++) {
            byte v = reference[i];
            has[v] = 1;
            if (v > max) {
                max = v;
            }
            if (v) {
                byte3 p = zox_tst_octree_position(i);
                if (p.x + 1 > bounds.x) bounds.x = p.x + 1;
                if (p.y + 1 > bounds.y) bounds.y = p.y + 1;
                if (p.z + 1 > bounds.z) bounds.z = p.z + 1;
            }
        }
        byte tree_max = get_max_value_TstNode(&root);
        zox_tst_check(tree_max == max, "[%i] max value [%i], expected [%i]", f, tree_max, max);
        for (byte v = 1; v < 12; v++) {
            byte is_in = is_in_TstNode(&root, v);
            zox_tst_check(is_in == has[v], "[%i] is_in_octree [%i] says [%i], expected [%i]", f, v, is_in, has[v]);
        }
        byte3 tree_bounds = get_bounds_TstNode(&root, zox_tst_octree_depth);
        zox_tst_check(byte3_equals(tree_bounds, bounds),
            "[%i] bounds [%ix%ix%i], expected [%ix%ix%i]", f,
            tree_bounds.x, tree_bounds.y, tree_bounds.z,
            bounds.x, bounds.y, bounds.z);
        zox_tst_octree_free(&root);
    }
    // a solid closed root fills the whole volume
    zox_tst_octree_init(&root, 1);
    byte3 full = get_bounds_TstNode(&root, zox_tst_octree_depth);
    zox_tst_check(full.x == zox_tst_octree_size && full.y == zox_tst_octree_size && full.z == zox_tst_octree_size,
        "solid root bounds [%ix%ix%i]", full.x, full.y, full.z);
}

// --- octree_neighbors: adjacent leaves inside a chunk and across to neighbour chunks ---
void zox_tst_octree_neighbors_check(ecs* world) {
    (void) world;
    const byte depth = 3;
    const byte size = 1 << depth;
    const byte max = size - 1;
    // center tree with a unique value per voxel, neighbours uniform (10 + dir)
    TstNode center;
    zox_tst_octree_init(&center, 0);
    for (byte x = 0; x < size; x++) for (byte y = 0; y < size; y++) for (byte z = 0; z < size; z++) {
        set_TstNode(&center, depth, (byte3) { x, y, z }, 1 + x + y * size + z * size * size % 100);
    }
    TstNode sides[6];
    const TstNode* neighbors[6];
    for (byte dir = 0; dir < 6; dir++) {
        zox_tst_octree_init(&sides[dir], 10 + dir);
        // give each neighbour one marked voxel on the face touching the center
        byte3 face = { 2, 5, 6 };
        byte axis = dir >> 1;
        byte touching = (dir & 1) ? 0 : max;
        if (axis == 0) face.x = touching; else if (axis == 1) face.y = touching; else face.z = touching;
        set_TstNode(&sides[dir], depth, face, 50 + dir);
        neighbors[dir] = &sides[dir];
    }
    int mismatches = 0;
    for (byte x = 0; x < size; x++) for (byte y = 0; y < size; y++) for (byte z = 0; z < size; z++) {
        for (byte dir = 0; dir < 6; dir++) {
            byte3 p = { x, y, z };
            int3 moved = move_position((int3) { x, y, z }, dir);
            byte expected;
            if (moved.x >= 0 && moved.x < size && moved.y >= 0 && moved.y < size && moved.z >= 0 && moved.z < size) {
                expected = getv_TstNode(&center, depth, int3_to_byte3(moved));
            } else {
                byte3 wrapped = int3_to_byte3(reverse_position(moved, dir, size));
                expected = getv_TstNode(&sides[dir], depth, wrapped);
            }
            const TstNode* n = get_neighbor_TstNode(&center, neighbors, dir, p, depth);
            if (!n || n->value != expected) {
                if (!mismatches) {
                    zox_loge("[tst] neighbor at [%ix%ix%i] dir [%i] gave [%i], expected [%i]",
                        x, y, z, dir, n ? n->value : -1, expected);
                }
                mismatches++;
            }
        }
    }
    zox_tst_check(!mismatches, "[%i] neighbor lookups wrong", mismatches);
    // the marked face voxels are found from the center's matching edge
    for (byte dir = 0; dir < 6; dir++) {
        byte3 p = { 2, 5, 6 };
        byte axis = dir >> 1;
        byte edge = (dir & 1) ? max : 0;
        if (axis == 0) p.x = edge; else if (axis == 1) p.y = edge; else p.z = edge;
        const TstNode* n = get_neighbor_TstNode(&center, neighbors, dir, p, depth);
        zox_tst_check(n && n->value == 50 + dir, "dir [%i] didn't find the marked face voxel, got [%i]", dir, n ? n->value : -1);
    }
    // no neighbours: edges give NULL, inside still works
    zox_tst_check(!get_neighbor_TstNode(&center, NULL, 0, (byte3) { 0, 3, 3 }, depth), "edge without neighbours returned a node");
    zox_tst_check(get_neighbor_TstNode(&center, NULL, 1, (byte3) { 0, 3, 3 }, depth) != NULL, "inside lookup without neighbours failed");
    const TstNode* missing[6] = { 0 };
    zox_tst_check(!get_neighbor_TstNode(&center, missing, 3, (byte3) { 1, max, 1 }, depth), "edge with a NULL neighbour returned a node");
    zox_tst_check(!get_neighbor_TstNode(&center, neighbors, 6, byte3_zero, depth), "invalid dir returned a node");
    zox_tst_octree_free(&center);
    for (byte dir = 0; dir < 6; dir++) {
        zox_tst_octree_free(&sides[dir]);
    }
    // is_on_edge_octree
    zox_tst_check(is_on_edge_octree(depth, (int3) { 0, 3, 3 }, direction_left), "left edge not detected");
    zox_tst_check(!is_on_edge_octree(depth, (int3) { 0, 3, 3 }, direction_right), "inside reported as edge");
    zox_tst_check(is_on_edge_octree(depth, (int3) { 3, 3, max }, direction_front), "front edge not detected");
}

// --- octree_nearby: the 27 chunk getv_nearby used by lights, with locks ---
void zox_tst_octree_nearby_check(ecs* world) {
    (void) world;
    const byte depth = 3;
    const byte size = 1 << depth;
    TstBasicNode trees[27];
    const TstBasicNode* octrees[27];
    spinlock lock_values[27];
    spinlock* locks[27];
    for (byte i = 0; i < 27; i++) {
        octree_create(&trees[i], sizeof(TstBasicNode), 100 + i);
        octrees[i] = &trees[i];
        spinlock_init(&lock_values[i]);
        locks[i] = &lock_values[i];
    }
    const byte center = OCTREE_INDEX(0, 0, 0);
    zox_tst_check(center == 13, "OCTREE_INDEX(0,0,0) is [%i]", center);
    set_TstBasicNode(&trees[center], depth, (byte3) { 4, 4, 4 }, 7);
    int mismatches = 0;
    for (sbyte ox = -1; ox <= 1; ox++) for (sbyte oy = -1; oy <= 1; oy++) for (sbyte oz = -1; oz <= 1; oz++) {
        byte3 positions[] = { { 0, 0, 0 }, { 7, 7, 7 }, { 4, 3, 4 }, { 0, 7, 3 } };
        for (byte k = 0; k < 4; k++) {
            byte3 p = positions[k];
            int rx = p.x + ox, ry = p.y + oy, rz = p.z + oz;
            sbyte cx = rx < 0 ? -1 : rx >= size ? 1 : 0;
            sbyte cy = ry < 0 ? -1 : ry >= size ? 1 : 0;
            sbyte cz = rz < 0 ? -1 : rz >= size ? 1 : 0;
            byte3 local = { (rx + size) % size, (ry + size) % size, (rz + size) % size };
            byte expected = getv_TstBasicNode(&trees[OCTREE_INDEX(cx, cy, cz)], depth, local);
            byte got = getv_nearby_TstBasicNode(octrees, locks, p, depth, (sbyte3) { ox, oy, oz }, 255);
            if (got != expected) {
                if (!mismatches) {
                    zox_loge("[tst] nearby [%ix%ix%i] + [%ix%ix%i] gave [%i], expected [%i]",
                        p.x, p.y, p.z, ox, oy, oz, got, expected);
                }
                mismatches++;
            }
        }
    }
    zox_tst_check(!mismatches, "[%i] nearby lookups wrong", mismatches);
    zox_tst_check(getv_nearby_TstBasicNode(octrees, locks, (byte3) { 4, 4, 3 }, depth, (sbyte3) { 0, 0, 1 }, 255) == 7,
        "nearby didn't read the center's detail voxel");
#ifndef zox_windows
    // locks are released after every read
    for (byte i = 0; i < 27; i++) {
        zox_tst_check(!atomic_flag_test_and_set(&lock_values[i].flag), "lock [%i] left locked", i);
        atomic_flag_clear(&lock_values[i].flag);
    }
#endif
    // missing chunk and too deep give the oob value
    octrees[OCTREE_INDEX(-1, 0, 0)] = NULL;
    zox_tst_check(getv_nearby_TstBasicNode(octrees, locks, (byte3) { 0, 2, 2 }, depth, (sbyte3) { -1, 0, 0 }, 77) == 77,
        "missing neighbour didn't give the oob value");
    zox_tst_check(getv_nearby_TstBasicNode(octrees, locks, byte3_zero, 7, sbyte3_zero, 66) == 66, "depth 7 didn't give the oob value");
    zox_tst_check(getv_nearby_TstBasicNode(NULL, locks, byte3_zero, depth, sbyte3_zero, 55) == 55, "NULL octrees didn't give the oob value");
    for (byte i = 0; i < 27; i++) {
        collapse_TstBasicNode(&trees[i]);
    }
}

// --- octree_links: linked (instance) nodes are leaves to every function, never children ---
//  - links at the leaf depth and a link higher up (depth 3, covering 4x4x4 voxels)

static inline int zox_tst_octree_count_links(const TstNode* node) {
    if (is_linked_TstNode(node)) {
        return 1;
    }
    const TstNode* kids = get_children_TstNode(node);
    if (!kids) {
        return 0;
    }
    int count = 0;
    for (byte i = 0; i < octree_length; i++) {
        count += zox_tst_octree_count_links(&kids[i]);
    }
    return count;
}

// links a node at depth, collapsing what's below it, and fills the reference with its value
static inline TstNode* zox_tst_octree_link_at(
    TstNode* root,
    byte* reference,
    byte depth,
    byte3 position,
    byte value,
    entity e)
{
    TstNode* node = open_TstNode(root, depth, position);
    if (!node) {
        return NULL;
    }
    collapse_TstNode(node);
    node->value = value;
    link_node_TstNode(node, e);
    const byte size = 1 << (zox_tst_octree_depth - depth);
    const byte3 start = { position.x * size, position.y * size, position.z * size };
    for (byte x = 0; x < size; x++) for (byte y = 0; y < size; y++) for (byte z = 0; z < size; z++) {
        reference[zox_tst_octree_index((byte3) { start.x + x, start.y + y, start.z + z })] = value;
    }
    return node;
}

void zox_tst_octree_links_check(ecs* world) {
    (void) world;
    static byte reference[zox_tst_octree_volume];
    TstNode root;
    zox_tst_octree_seed = 8080;
    zox_tst_octree_fill_random(&root, reference, 6000, 3);
    // one link high up, at depth 3 position 1x2x3 covers voxels [4..7]x[8..11]x[12..15]
    TstNode* high = zox_tst_octree_link_at(&root, reference, 3, (byte3) { 1, 2, 3 }, 7, 777);
    zox_tst_check(high && is_linked_TstNode(high), "high link wasn't made");
    // links on leaves
    TstNode* leaves[16];
    byte3 leaf_positions[16];
    int leaves_count = 0;
    for (int i = 0; i < 16; i++) {
        byte3 p = zox_tst_octree_random_position();
        // keep out of the high link
        if (p.x >= 4 && p.x < 8 && p.y >= 8 && p.y < 12 && p.z >= 12 && p.z < 16) {
            continue;
        }
        // and off already linked leaves
        const TstNode* existing = get_TstNode(&root, zox_tst_octree_depth, p);
        if (existing && is_linked_TstNode(existing)) {
            continue;
        }
        TstNode* leaf = zox_tst_octree_link_at(&root, reference, zox_tst_octree_depth, p, 4, 100 + i);
        if (leaf && is_linked_TstNode(leaf)) {
            leaf_positions[leaves_count] = p;
            leaves[leaves_count++] = leaf;
        }
    }
    const int links = zox_tst_octree_count_links(&root);
    zox_tst_check(links == 1 + leaves_count, "tree has [%i] links, expected [%i]", links, 1 + leaves_count);
    zox_tst_octree_compare(&root, reference, "linked");
    // reads under the high link stop at it
    zox_tst_check(get_TstNode(&root, zox_tst_octree_depth, (byte3) { 5, 9, 13 }) == high, "read under the high link didn't stop at it");
    zox_tst_check(get_TstNode(&root, 3, (byte3) { 1, 2, 3 }) == high, "read at the high link's depth didn't find it");
    zox_tst_check(get_node_entity_TstNode(high) == 777, "high link entity [%lu]", (unsigned long) get_node_entity_TstNode(high));
    // writes can't open through a link
    zox_tst_check(!set_TstNode(&root, zox_tst_octree_depth, (byte3) { 5, 9, 13 }, 2), "set opened through the high link");
    zox_tst_check(is_linked_TstNode(high) && high->value == 7, "set under the high link changed it");
    zox_tst_octree_compare(&root, reference, "after blocked set");
    // reduce + optimize keep every link
    reduce_TstNode(&root);
    zox_tst_check(zox_tst_octree_count_links(&root) == links, "reduce lost links, [%i] of [%i]", zox_tst_octree_count_links(&root), links);
    zox_tst_octree_compare(&root, reference, "linked reduced");
    optimize_TstNode(&root);
    optimize_reduce_TstNode(&root);
    zox_tst_check(zox_tst_octree_count_links(&root) == links, "optimize_reduce lost links");
    zox_tst_octree_compare(&root, reference, "linked optimized");
    for (int i = 0; i < leaves_count; i++) {
        const TstNode* leaf = get_TstNode(&root, zox_tst_octree_depth, leaf_positions[i]);
        zox_tst_check(leaf == leaves[i] && get_node_entity_TstNode(leaf) == (entity) (100 + i),
            "leaf link [%i] moved or lost its entity", i);
    }
    // queries treat a link as a leaf with its value
    byte max = 0;
    byte3 bounds = byte3_zero;
    for (int i = 0; i < zox_tst_octree_volume; i++) {
        if (reference[i] > max) {
            max = reference[i];
        }
        if (reference[i]) {
            byte3 p = zox_tst_octree_position(i);
            if (p.x + 1 > bounds.x) bounds.x = p.x + 1;
            if (p.y + 1 > bounds.y) bounds.y = p.y + 1;
            if (p.z + 1 > bounds.z) bounds.z = p.z + 1;
        }
    }
    zox_tst_check(get_max_value_TstNode(&root) == max, "linked max [%i], expected [%i]", get_max_value_TstNode(&root), max);
    zox_tst_check(is_in_TstNode(&root, 7), "linked is_in didn't find the high link's value");
    byte3 tree_bounds = get_bounds_TstNode(&root, zox_tst_octree_depth);
    zox_tst_check(byte3_equals(tree_bounds, bounds), "linked bounds [%ix%ix%i], expected [%ix%ix%i]",
        tree_bounds.x, tree_bounds.y, tree_bounds.z, bounds.x, bounds.y, bounds.z);
    // neighbours of the high link find it, not memory behind it
    const TstNode* next = get_neighbor_TstNode(&root, NULL, direction_right, (byte3) { 3, 9, 13 }, zox_tst_octree_depth);
    zox_tst_check(next == high, "neighbor into the high link didn't return it");
    // clone drops links, keeps values
    TstNode copy;
    clone_TstNode(&copy, &root);
    zox_tst_check(zox_tst_octree_count_links(&copy) == 0, "clone kept [%i] links", zox_tst_octree_count_links(&copy));
    zox_tst_octree_compare(&copy, reference, "linked clone");
    collapse_TstNode(&copy);
    // unlink, then the region can be written again
    zox_tst_check(dispose_node_link_TstNode(high), "dispose didn't unlink the high link");
    zox_tst_check(!is_linked_TstNode(high) && !high->ptr && high->value == 7, "dispose left the high link behind");
    zox_tst_check(set_TstNode(&root, zox_tst_octree_depth, (byte3) { 5, 9, 13 }, 2) != NULL, "set after unlinking failed");
    reference[zox_tst_octree_index((byte3) { 5, 9, 13 })] = 2;
    zox_tst_octree_compare(&root, reference, "after unlink");
    // close and collapse free links (leaks are checked by octree_leaks)
    close_TstNode(&root);
    zox_tst_check(!root.ptr && !root.type, "close left children or a type");
    // a linked root
    zox_tst_octree_init(&root, 3);
    link_node_TstNode(&root, 55);
    zox_tst_check(getv_TstNode(&root, zox_tst_octree_depth, (byte3) { 9, 9, 9 }) == 3, "linked root didn't read as a leaf");
    zox_tst_check(!set_TstNode(&root, 2, byte3_zero, 1), "set opened a linked root");
    zox_tst_check(!reduce_TstNode(&root) && is_linked_TstNode(&root), "reduce changed a linked root");
    clone_TstNode(&copy, &root);
    zox_tst_check(!copy.ptr && !copy.type && copy.value == 3, "clone of a linked root kept the link");
    collapse_TstNode(&root);
    zox_tst_check(!root.ptr && !root.type && root.value == 3, "collapse of a linked root didn't unlink it");
}

// --- octree_leaks: every way of freeing a tree gives back all its memory (glibc only) ---
#ifdef __GLIBC__
#include <malloc.h>

static inline long zox_tst_octree_heap() {
    struct mallinfo2 info = mallinfo2();
    return (long) info.uordblks;
}

// builds a deep random tree, values in [0, 3) so reduce has work to do
static inline void zox_tst_octree_build(TstNode* root, int count) {
    zox_tst_octree_init(root, 0);
    for (int i = 0; i < count; i++) {
        set_TstNode(root, zox_tst_octree_depth, zox_tst_octree_random_position(), zox_tst_octree_rand() % 3);
    }
}

// runs a build + free many times, a leak grows the heap by far more than the slack
#define zox_tst_octree_leak_rounds 20
#define zox_tst_octree_leak_slack 16384

static inline void zox_tst_octree_leak_check(const char* label, int mode) {
    zox_tst_octree_seed = 555;
    TstNode root;
    TstNode copy;
    // warm up so malloc's own bookkeeping is allocated
    zox_tst_octree_build(&root, 3000);
    zox_tst_octree_free(&root);
    const long before = zox_tst_octree_heap();
    long built = 0;
    for (int r = 0; r < zox_tst_octree_leak_rounds; r++) {
        zox_tst_octree_build(&root, 3000);
        if (!r) {
            built = zox_tst_octree_heap() - before;
        }
        if (mode == 0) {
            close_TstNode(&root);
        } else if (mode == 1) {
            collapse_TstNode(&root);
        } else if (mode == 2) {
            move_TstNode(&copy, &root);
            collapse_TstNode(&copy);
        } else if (mode == 3) {
            reduce_TstNode(&root);
            collapse_TstNode(&root);
        } else if (mode == 4) {
            clone_TstNode(&copy, &root);
            collapse_TstNode(&root);
            collapse_TstNode(&copy);
        } else if (mode == 5) {
            optimize_reduce_TstNode(&root);
            collapse_TstNode(&root);
        } else if (mode == 6) {
            // links on random leaves, collapse must free them too
            for (int l = 0; l < 200; l++) {
                TstNode* leaf = set_TstNode(&root, zox_tst_octree_depth, zox_tst_octree_random_position(), 1);
                if (leaf && !leaf->ptr) {
                    link_node_TstNode(leaf, 1000 + l);
                }
            }
            collapse_TstNode(&root);
        }
        zox_tst_check(!root.ptr, "[%s] left children on the root", label);
    }
    const long grown = zox_tst_octree_heap() - before;
    zox_tst_check(built > zox_tst_octree_leak_slack, "[%s] building only used [%li] bytes, test can't see leaks", label, built);
    zox_tst_check(grown < zox_tst_octree_leak_slack,
        "[%s] leaked [%li] bytes over [%i] rounds (one tree is [%li] bytes)",
        label, grown, zox_tst_octree_leak_rounds, built);
}

void zox_tst_octree_leaks_check(ecs* world) {
    (void) world;
    zox_tst_octree_leak_check("close", 0);
    zox_tst_octree_leak_check("collapse", 1);
    zox_tst_octree_leak_check("move", 2);
    zox_tst_octree_leak_check("reduce", 3);
    zox_tst_octree_leak_check("clone", 4);
    zox_tst_octree_leak_check("optimize_reduce", 5);
    zox_tst_octree_leak_check("links", 6);
    // close keeps the value
    TstNode root;
    zox_tst_octree_init(&root, 6);
    set_TstNode(&root, zox_tst_octree_depth, (byte3) { 1, 2, 3 }, 9);
    close_TstNode(&root);
    zox_tst_check(!root.ptr && root.value == 6, "close lost the value, [%i]", root.value);
}
#else
void zox_tst_octree_leaks_check(ecs* world) {
    (void) world;
    zox_log("[tst] octree_leaks needs glibc mallinfo2, skipped");
}
#endif

void zox_tsts_octrees() {
    const double timeout = 30;
    zox_tst_add("octree_basics", zox_tst_octrees_ready, zox_tst_octree_basics_check, timeout);
    zox_tst_add("octree_set_get", zox_tst_octrees_ready, zox_tst_octree_set_get_check, timeout);
    zox_tst_add("octree_reduce", zox_tst_octrees_ready, zox_tst_octree_reduce_check, timeout);
    zox_tst_add("octree_optimize", zox_tst_octrees_ready, zox_tst_octree_optimize_check, timeout);
    zox_tst_add("octree_clone_move", zox_tst_octrees_ready, zox_tst_octree_clone_move_check, timeout);
    zox_tst_add("octree_queries", zox_tst_octrees_ready, zox_tst_octree_queries_check, timeout);
    zox_tst_add("octree_neighbors", zox_tst_octrees_ready, zox_tst_octree_neighbors_check, timeout);
    zox_tst_add("octree_nearby", zox_tst_octrees_ready, zox_tst_octree_nearby_check, timeout);
    zox_tst_add("octree_links", zox_tst_octrees_ready, zox_tst_octree_links_check, timeout);
    zox_tst_add("octree_leaks", zox_tst_octrees_ready, zox_tst_octree_leaks_check, timeout);
}
