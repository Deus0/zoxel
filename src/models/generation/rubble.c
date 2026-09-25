void build_vox_heights(
    VoxelNode* voxels,
    byte depth,
    byte2 range,
    byte max_height)
{
    short length = octree_size(depth);
    byte3 position;
    for (position.x = 0; position.x < length; position.x++) {
        for (position.z = 0; position.z < length; position.z++) {
            byte height = rand_range(0, max_height);
            for (position.y = 0; position.y < height; position.y++) {
                byte value = rand_range(range.x, range.y);
                set_voxel_safe(voxels, depth, position, value);
            }
        }
    }
}

// NOTE: This is quite laggy atm due to the large amounts of rand()s
// NOTE: Simulates more natural random placement
/*void build_vox_scatter(
    VoxelNode* voxels,
    byte depth,
    byte2 range,
    byte max_height,
    uint count)
{
    byte length = octree_size(depth);
    byte heights[length][length];
    memset(heights, 0, sizeof(heights));
    for (uint i = 0; i < count; i++) {
        byte x = rand() % length;
        byte z = rand() % length;
        if (heights[x][z] < max_height) {
            heights[x][z]++;
        }
    }
    byte3 position;
    for (position.x = 0; position.x < length; position.x++) {
        for (position.z = 0; position.z < length; position.z++) {
            byte height = heights[position.x][position.z];
            for (position.y = 0; position.y < height; position.y++) {
                byte value = rand_range(range.x, range.y);
                set_voxel_safe(voxels, depth, position, value);
            }
        }
    }
}*/

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

void build_vox_scatter(
    VoxelNode* voxels,
    byte depth,
    byte2 range,
    byte max_height,
    uint count)
{
    byte length = octree_size(depth);
    byte heights[length][length];
    memset(heights, 0, sizeof(heights));

    uint state = 0x9e3779b9u;

    uint seed = (uint) count;
    seed ^= ((uint) depth << 24);
    seed ^= ((uint) range.x << 16);
    seed ^= ((uint) range.y << 8);
    seed ^= max_height;
    state ^= seed * 747796405u;

    uint mask = (uint) length - 1;

    uint max_count = (uint) length * (uint) length * (uint) max_height;
    if (count > max_count) {
        count = max_count;
    }

    for (uint i = 0; i < count; i++) {
        uint random = scatter_rand(&state);

        byte x = (byte)(random & mask);
        byte z = (byte)((random >> 8) & mask);

        if (heights[x][z] < max_height) {
            heights[x][z]++;
        }
        else {
            i--;
        }
    }

    byte3 position;

    for (position.x = 0; position.x < length; position.x++) {
        for (position.z = 0; position.z < length; position.z++) {
            byte height = heights[position.x][position.z];

            for (position.y = 0; position.y < height; position.y++) {
                byte value = scatter_rand_byte(
                    &state,
                    range.x,
                    range.y);

                set_voxel_safe(
                    voxels,
                    depth,
                    position,
                    value);
            }
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
        srand(seed->value);
        for (int j = 0; j < unique_colors; j++) {
            color_rgb new_color = color_rgb_2;
            float m = randf_range(color_r.x, color_r.y);
            color_rgb_multiply_float(&new_color, m);
            add_to_ColorRGBs(colors, new_color);
        }
        byte max_height = zox_has(e, RubbleHeight) ? zox_getv(e, RubbleHeight) : 4;
        // Write Locks node
        // write_lock_VoxelNode(node);
        // build_vox_heights(node, node_depth, vrange, max_height);
        uint count = zox_has(e, RubbleCount) ?
            zox_getv(e, RubbleCount) :
            200;
        build_vox_scatter(
            node,
            node_depth,
            vrange,
            max_height,
            count);
        // Outlines
        if (zox_block_outlines) {
            add_to_ColorRGBs(colors, color_rgb_black);
            byte black_voxel = colors->length;
            vox_outlines(node, depth->value, black_voxel);
        }
        // write_unlock_VoxelNode(node);
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
