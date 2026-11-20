static inline uint scatter_rand(uint* state) {
    uint x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static inline byte scatter_rand_byte(
    uint* state,
    byte min,
    byte max)
{
    return min + (byte)(scatter_rand(state) % (uint)(max - min + 1));
}

// tufts of grass: count stalks, each a column of its own (stalks between them, not every column filled: a carpet of
//  them looked like a mass), tallest near the block's middle, seeded by the model
//  - a stalk's height: max_height times 0.35 to 1, less out towards the edges (half at a corner)
//  - its voxels a colour each from range, so a stalk shades along it
void build_vox_tufts(
    VoxelNode* voxels,
    byte depth,
    byte2 range,
    byte max_height,
    uint count,
    uint seed)
{
    const byte length = octree_size(depth);
    const uint mask = (uint) length - 1;
    uint state = 0x9e3779b9u ^ (seed * 747796405u) ^ ((uint) depth << 24);
    if (count > (uint) length * length) {
        count = (uint) length * length;
    }
    byte filled[length][length];
    memset(filled, 0, sizeof(filled));
    const float half = (length - 1) * 0.5f;
    uint tries = count * 8;
    for (uint placed = 0; placed < count && tries; tries--) {
        const uint random = scatter_rand(&state);
        const byte x = (byte) (random & mask);
        const byte z = (byte) ((random >> 8) & mask);
        if (filled[x][z]) {
            continue;
        }
        filled[x][z] = 1;
        placed++;
        const float out = float_max(float_abs(x - half), float_abs(z - half)) / half;    // 0 middle, 1 edge
        const float tall = (0.35f + 0.65f * (scatter_rand(&state) % 1000) / 1000.0f) * (1 - 0.5f * out);
        const byte height = (byte) int_max(1, (int) (max_height * tall));
        for (byte y = 0; y < height; y++) {
            set_voxel_safe(voxels, depth, (byte3) { x, y, z }, scatter_rand_byte(&state, range.x, range.y));
        }
    }
}

void rubble_model_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Seed);
    zox_sys_in(Color);
    zox_sys_in(VoxType);
    zox_sys_out(GenerateModel);
    zox_sys_out(VoxelNode);
    zox_sys_out(NodeDepth);
    zox_sys_out(ColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Seed, seed);
        zox_sys_i(Color, fill);
        zox_sys_i(VoxType, gentype);
        zox_sys_o(GenerateModel, generate);
        zox_sys_o(VoxelNode, node);
        zox_sys_o(NodeDepth, depth);
        zox_sys_o(ColorRGBs, colors);
        if (generate->value != zox_generate_model_run) {
            continue;
        }
        if (gentype->value != vox_type_rubble) {
            continue;
        }
        resize_ColorRGBs(colors, 0);
        byte unique_colors = zox_has(e, VoxUniqueColors) ? zox_getv(e, VoxUniqueColors) : default_unique_colors;
        float color_rr = zox_has(e, VoxColorRange) ? zox_getv(e, VoxColorRange) : default_color_range;
        float2 color_r = (float2) { 1 - color_rr, 1 + color_rr };
        byte node_depth = depth->value;
        color_rgb color_rgb_2 = color_to_color_rgb(fill->value);
        // colors to pick from
        byte2 vrange = (byte2) { 1, unique_colors - 1 };
        zox_srand(seed->value);
        for (int j = 0; j < unique_colors; j++) {
            color_rgb new_color = color_rgb_2;
            float m = randf_range(color_r.x, color_r.y);
            color_rgb_multiply_float(&new_color, m);
            add_to_ColorRGBs(colors, new_color);
        }
        byte max_height = zox_has(e, RubbleHeight) ? zox_getv(e, RubbleHeight) : 4;
        // Write Locks node
        // its octree locked while written, readers can run alongside (thread_safety.md)
        spinlock* write_lock = zox_octree_lock(world, e);
        zox_octree_lock_begin(write_lock);
        uint count = zox_has(e, RubbleCount) ?
            zox_getv(e, RubbleCount) :
            200;
        build_vox_tufts(
            node,
            node_depth,
            vrange,
            max_height,
            count,
            (uint) seed->value);
        // Outlines
        if (zox_block_outlines) {
            add_to_ColorRGBs(colors, color_rgb_black);
            byte black_voxel = colors->length;
            vox_outlines(node, depth->value, black_voxel);
        }
        zox_octree_lock_end(write_lock);
        generate->value = zox_has(e, BakeModel) ?
            zox_generate_model_bake :
            zox_generate_model_end;
        zox_add(e, VoxelNodeDirty);
        if (dbg_log) {
            zox_log("Generated Vox [%s] Type [%i] Depth [%i]", zox_get_name(e), gentype->value, depth->value);
        }
    }
    zox_sys_on_end();
} zoxd_system(rubble_model_system);
