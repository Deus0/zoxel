// Block tables: a voxel is its block's index in its realm's BlockLinks, those indices change when blocks are added or
//  reordered, so saved chunks read as other blocks. A realm saves blocks.txt (each index's block: name and seed) and
//  once its blocks are indexed (BlockTableDirty, after generation) remaps its saved chunks if any index moved.
//  - a block gone from the realm loads as air, a save without a table loads as it is
//  - main thread, it reads and writes files, once a realm a game
#define block_table_max 255
#define block_table_key 96

// a block's key: its name and seed (names repeat across biomes, seeds within one: dirt and debris)
static inline void block_table_key_of(ecs* world, entity block, char* out) {
    const char* name = zox_valid(block) && zox_has(block, ZoxName) ? zox_get(block, ZoxName)->value : NULL;
    snprintf(out, block_table_key, "%s %lld", name && name[0] ? name : "-",
        zox_valid(block) && zox_has(block, Seed) ? (long long) zox_getv(block, Seed) : 0ll);
}

// reads blocks.txt, a key a line, line n is index n, returns how many (0 if none)
static inline int block_table_read(const char* path, char keys[block_table_max][block_table_key]) {
    FILE* file = fopen(path, "r");
    if (!file) {
        return 0;
    }
    int count = 0;
    while (count < block_table_max && fgets(keys[count], block_table_key, file)) {
        keys[count][strcspn(keys[count], "\r\n")] = 0;
        count++;
    }
    fclose(file);
    return count;
}

static inline void remap_VoxelNode(VoxelNode* node, const byte* remap) {
    node->value = remap[node->value];
    if (has_children_VoxelNode(node)) {
        VoxelNode* children = get_children_VoxelNode(node);
        for (int i = 0; i < 8; i++) {
            remap_VoxelNode(&children[i], remap);
        }
    }
}

// rewrites a saved chunk through the remap, 1 if it did
static inline byte block_table_remap_chunk(ecs* world, const char* path, const byte* remap) {
    FILE* in = fopen(path, "rb");
    if (!in) {
        return 0;
    }
    VoxelNode node;
    octree_create(&node, sizeof(VoxelNode), 0);
    const byte loaded = load_voxel_node(world, in, &node);
    fclose(in);
    if (loaded) {
        remap_VoxelNode(&node, remap);
        FILE* out = fopen(path, "wb");
        if (out) {
            save_voxel_node(out, &node);
            fclose(out);
        }
    }
    close_VoxelNode(&node);
    return loaded;
}

void block_table_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(BlockLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, generate);
        zox_sys_i(BlockLinks, blocks);
        // after generation, when every block is in
        if (generate->value || !zox_has(e, FolderPath)) {
            continue;
        }
        zox_remove(e, BlockTableDirty);
        const FolderPath* folder = zox_get(e, FolderPath);
        if (!folder->value || !folder->value[0]) {
            continue;
        }
        char* path = join_path(folder->value, "blocks.txt");
        static char saved[block_table_max][block_table_key];
        static char current[block_table_max][block_table_key];
        const int saved_count = block_table_read(path, saved);
        const int current_count = blocks->length < block_table_max ? blocks->length : block_table_max;
        for (int j = 0; j < current_count; j++) {
            block_table_key_of(world, blocks->value[j], current[j]);
        }
        // each saved index to its block's current index, gone blocks to air
        byte remap[256];
        for (int j = 0; j < 256; j++) {
            remap[j] = j;
        }
        byte changed = 0;
        for (int j = 0; j < saved_count; j++) {
            byte to = 0;
            for (int k = 0; k < current_count && !to; k++) {
                if (!strcmp(saved[j], current[k])) {
                    to = k + 1;
                }
            }
            remap[j + 1] = to;
            changed |= to != j + 1;
        }
        if (saved_count && changed) {
            FileList files = get_files(folder->value, 1);
            int remapped = 0;
            for (int f = 0; f < files.count; f++) {
                if (!strncmp(files.filenames[f], "chunk_", 6)) {
                    remapped += block_table_remap_chunk(world, files.files[f], remap);
                }
            }
            free_files(&files);
            zox_log("[block_table] [%s] block indices moved, [%i] saved chunks remapped", zox_get_name(e), remapped);
        }
        FILE* file = fopen(path, "w");
        if (file) {
            for (int j = 0; j < current_count; j++) {
                fprintf(file, "%s\n", current[j]);
            }
            fclose(file);
        }
        free(path);
    }
    zox_sys_on_end();
} zoxd_system(block_table_system);
