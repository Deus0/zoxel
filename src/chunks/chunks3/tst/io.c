// tst: chunk_io, a chunk's VoxelNode saved to a file and loaded back is the same tree
//  - random trees, compared voxel by voxel with the original (and their node counts)
//  - loading over an existing, different tree replaces it (opens and closes branches)
//  - a truncated file fails without crashing
//  - saving twice gives the same bytes
//  - run: bash bsh/tst.sh chunk_io
#define zox_tst_io_depth 5
#define zox_tst_io_size (1 << zox_tst_io_depth)
#define zox_tst_io_volume (zox_tst_io_size * zox_tst_io_size * zox_tst_io_size)

uint zox_tst_io_seed = 4321;

static inline uint zox_tst_io_rand() {
    uint x = zox_tst_io_seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    zox_tst_io_seed = x;
    return x;
}

static inline byte3 zox_tst_io_position(int i) {
    return (byte3) { i % zox_tst_io_size, (i / zox_tst_io_size) % zox_tst_io_size, i / (zox_tst_io_size * zox_tst_io_size) };
}

static inline void zox_tst_io_random_tree(VoxelNode* node, int count, byte values) {
    create_VoxelNode(node);
    for (int i = 0; i < count; i++) {
        set_VoxelNode(node, zox_tst_io_depth, zox_tst_io_position(zox_tst_io_rand() % zox_tst_io_volume), zox_tst_io_rand() % values);
    }
    reduce_VoxelNode(node);
}

static inline int zox_tst_io_count_nodes(const VoxelNode* node) {
    const VoxelNode* kids = get_children_VoxelNode(node);
    if (!kids) {
        return 1;
    }
    int count = 1;
    for (int i = 0; i < octree_length; i++) {
        count += zox_tst_io_count_nodes(&kids[i]);
    }
    return count;
}

static inline int zox_tst_io_differences(const VoxelNode* a, const VoxelNode* b) {
    int differences = 0;
    for (int i = 0; i < zox_tst_io_volume; i++) {
        const byte3 p = zox_tst_io_position(i);
        differences += getv_VoxelNode(a, zox_tst_io_depth, p) != getv_VoxelNode(b, zox_tst_io_depth, p);
    }
    return differences;
}

// saves to a temp file, returns its bytes (caller frees), size in size_out
static inline byte* zox_tst_io_save(const VoxelNode* node, long* size_out) {
    FILE* file = tmpfile();
    if (!file) {
        return NULL;
    }
    save_voxel_node(file, node);
    *size_out = ftell(file);
    rewind(file);
    byte* bytes = malloc(*size_out > 0 ? *size_out : 1);
    if (fread(bytes, 1, *size_out, file) != (size_t) *size_out) {
        free(bytes);
        bytes = NULL;
    }
    fclose(file);
    return bytes;
}

static inline byte zox_tst_io_load(ecs* world, const byte* bytes, long size, VoxelNode* node) {
    FILE* file = tmpfile();
    if (!file) {
        return 0;
    }
    fwrite(bytes, 1, size, file);
    rewind(file);
    byte loaded = load_voxel_node(world, file, node);
    fclose(file);
    return loaded;
}

byte zox_tst_chunk_io_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_chunk_io_check(ecs* world) {
    const int fills[] = { 0, 20, 800, 12000 };
    for (int f = 0; f < 4; f++) {
        VoxelNode original;
        zox_tst_io_random_tree(&original, fills[f], 4);
        long size = 0;
        byte* bytes = zox_tst_io_save(&original, &size);
        zox_tst_check(bytes, "[%i] save failed", fills[f]);
        if (!bytes) {
            collapse_VoxelNode(&original);
            continue;
        }
        // a node saves as 2 bytes (value, has children)
        const int nodes = zox_tst_io_count_nodes(&original);
        zox_tst_check(size == nodes * 2, "[%i] saved [%li] bytes for [%i] nodes", fills[f], size, nodes);
        // load into a fresh node
        VoxelNode loaded;
        create_VoxelNode(&loaded);
        zox_tst_check(zox_tst_io_load(world, bytes, size, &loaded), "[%i] load failed", fills[f]);
        zox_tst_check(!zox_tst_io_differences(&original, &loaded), "[%i] fresh load differs in [%i] voxels", fills[f], zox_tst_io_differences(&original, &loaded));
        zox_tst_check(zox_tst_io_count_nodes(&loaded) == nodes, "[%i] fresh load has [%i] nodes, saved [%i]", fills[f], zox_tst_io_count_nodes(&loaded), nodes);
        // save the loaded tree again, same bytes
        long size2 = 0;
        byte* bytes2 = zox_tst_io_save(&loaded, &size2);
        zox_tst_check(bytes2 && size2 == size && !memcmp(bytes, bytes2, size), "[%i] saving the loaded tree gave different bytes", fills[f]);
        free(bytes2);
        // load over a different existing tree, it must be replaced completely
        VoxelNode existing;
        zox_tst_io_random_tree(&existing, 6000, 7);
        zox_tst_check(zox_tst_io_load(world, bytes, size, &existing), "[%i] load over existing failed", fills[f]);
        zox_tst_check(!zox_tst_io_differences(&original, &existing), "[%i] load over an existing tree differs in [%i] voxels", fills[f], zox_tst_io_differences(&original, &existing));
        zox_tst_check(zox_tst_io_count_nodes(&existing) == nodes, "[%i] load over existing has [%i] nodes, saved [%i] (old branches left open)", fills[f], zox_tst_io_count_nodes(&existing), nodes);
        // truncated files fail cleanly
        if (size > 4) {
            VoxelNode truncated;
            create_VoxelNode(&truncated);
            zox_tst_io_load(world, bytes, size / 2, &truncated);
            collapse_VoxelNode(&truncated);
            VoxelNode empty;
            create_VoxelNode(&empty);
            zox_tst_check(!zox_tst_io_load(world, bytes, 0, &empty), "[%i] loading an empty file reported success", fills[f]);
            collapse_VoxelNode(&empty);
        }
        free(bytes);
        collapse_VoxelNode(&original);
        collapse_VoxelNode(&loaded);
        collapse_VoxelNode(&existing);
    }
}

// an empty BlockDatas (its lock ready)
static inline BlockDatas zox_tst_datas_new() {
    BlockDatas datas = { 0 };
    spinlock_init(&datas.lock);
    return datas;
}

// tst: block_table_remap, a saved chunk rewritten through a block remap (io/sys/block_table.c): each voxel its new
//  index, a removed block air, the rest as they were, its block datas kept
//  - run: bash bsh/tst.sh block_table_remap
void zox_tst_block_table_remap_check(ecs* world) {
    VoxelNode original;
    zox_tst_io_random_tree(&original, 3000, 5);
    const char* path = zox_tst_path("block_table_remap.dat");
    FILE* out = fopen(path, "wb");
    zox_tst_check(out != NULL, "couldn't write [%s]", path);
    if (!out) {
        return;
    }
    save_voxel_node(out, &original);
    // a block data, kept through the remap
    BlockDatas datas = zox_tst_datas_new();
    set_block_data(&datas, (byte3) { 3, 4, 5 }, 6);
    save_block_datas(out, &datas);
    dispose_BlockDatas(&datas);
    fclose(out);
    byte remap[256];
    for (int i = 0; i < 256; i++) {
        remap[i] = i;
    }
    remap[1] = 3;   // swapped
    remap[3] = 1;
    remap[4] = 0;   // removed, air
    zox_tst_check(block_table_remap_chunk(world, path, remap), "the chunk didn't remap");
    FILE* in = fopen(path, "rb");
    VoxelNode loaded;
    create_VoxelNode(&loaded);
    load_voxel_node(world, in, &loaded);
    BlockDatas loaded_datas = zox_tst_datas_new();
    load_block_datas(in, &loaded_datas);
    fclose(in);
    zox_tst_check(get_block_data(&loaded_datas, (byte3) { 3, 4, 5 }) == 6, "its block data wasn't kept through the remap");
    dispose_BlockDatas(&loaded_datas);
    int wrong = 0;
    for (int i = 0; i < zox_tst_io_volume; i++) {
        const byte3 p = zox_tst_io_position(i);
        wrong += getv_VoxelNode(&loaded, zox_tst_io_depth, p) != remap[getv_VoxelNode(&original, zox_tst_io_depth, p)];
    }
    zox_tst_check(!wrong, "[%i] voxels didn't remap", wrong);
    collapse_VoxelNode(&original);
    collapse_VoxelNode(&loaded);
    remove(path);
}

// tst: block_datas_io, a chunk's block datas (fun/block_data.c) saved after its octree and loaded back
//  - set, changed and removed by position (set_block_data, 0 removes)
//  - a chunk with two saves 3 + 2 * 4 bytes more than its octree, both back the same after loading
//  - a chunk with none saves its octree's bytes only (as before), loads none; an older file (octree only) loads none
//  - run: bash bsh/tst.sh block_datas_io
void zox_tst_block_datas_io_check(ecs* world) {
    BlockDatas datas = zox_tst_datas_new();
    set_block_data(&datas, (byte3) { 1, 2, 3 }, 5);
    set_block_data(&datas, (byte3) { 4, 5, 6 }, 9);
    set_block_data(&datas, (byte3) { 7, 7, 7 }, 2);
    set_block_data(&datas, (byte3) { 4, 5, 6 }, 10);
    set_block_data(&datas, (byte3) { 7, 7, 7 }, 0);
    zox_tst_check(datas.length == 2 && get_block_data(&datas, (byte3) { 1, 2, 3 }) == 5 &&
        get_block_data(&datas, (byte3) { 4, 5, 6 }) == 10 && !get_block_data(&datas, (byte3) { 7, 7, 7 }),
        "set, changed and removed: [%i] entries, values [%i %i %i]", datas.length, get_block_data(&datas, (byte3) { 1, 2, 3 }),
        get_block_data(&datas, (byte3) { 4, 5, 6 }), get_block_data(&datas, (byte3) { 7, 7, 7 }));
    VoxelNode node;
    zox_tst_io_random_tree(&node, 800, 4);
    long octree_size_bytes = 0;
    byte* octree_bytes = zox_tst_io_save(&node, &octree_size_bytes);
    // with block datas
    FILE* file = tmpfile();
    save_voxel_node(file, &node);
    save_block_datas(file, &datas);
    const long size = ftell(file);
    zox_tst_check(size == octree_size_bytes + 3 + 2 * 4, "with two block datas it's [%li] bytes, its octree [%li]", size, octree_size_bytes);
    rewind(file);
    VoxelNode loaded;
    create_VoxelNode(&loaded);
    BlockDatas loaded_datas = zox_tst_datas_new();
    const byte ok = load_voxel_node(world, file, &loaded) && load_block_datas(file, &loaded_datas);
    fclose(file);
    zox_tst_check(ok, "loading a chunk with block datas failed");
    zox_tst_check(!zox_tst_io_differences(&node, &loaded), "its octree loaded back different");
    zox_tst_check(loaded_datas.length == 2 && get_block_data(&loaded_datas, (byte3) { 1, 2, 3 }) == 5 &&
        get_block_data(&loaded_datas, (byte3) { 4, 5, 6 }) == 10, "its block datas loaded back as [%i] entries", loaded_datas.length);
    // with none: its octree only, as before
    BlockDatas none = zox_tst_datas_new();
    file = tmpfile();
    save_voxel_node(file, &node);
    save_block_datas(file, &none);
    const long none_size = ftell(file);
    zox_tst_check(none_size == octree_size_bytes, "with no block datas it's [%li] bytes, not its octree's [%li]", none_size, octree_size_bytes);
    fclose(file);
    // an older file, octree only, loads none
    file = tmpfile();
    fwrite(octree_bytes, 1, octree_size_bytes, file);
    rewind(file);
    VoxelNode old;
    create_VoxelNode(&old);
    BlockDatas old_datas = zox_tst_datas_new();
    zox_tst_check(load_voxel_node(world, file, &old) && load_block_datas(file, &old_datas) && !old_datas.length,
        "an older file loaded [%i] block datas", old_datas.length);
    fclose(file);
    free(octree_bytes);
    dispose_BlockDatas(&datas);
    dispose_BlockDatas(&loaded_datas);
    dispose_BlockDatas(&old_datas);
    collapse_VoxelNode(&node);
    collapse_VoxelNode(&loaded);
    collapse_VoxelNode(&old);
}

void zox_tsts_chunks3_io() {
    zox_tst_add("block_datas_io", zox_tst_chunk_io_ready, zox_tst_block_datas_io_check, 30);
    zox_tst_add("chunk_io", zox_tst_chunk_io_ready, zox_tst_chunk_io_check, 30);
    zox_tst_add("block_table_remap", zox_tst_chunk_io_ready, zox_tst_block_table_remap_check, 30);
}
