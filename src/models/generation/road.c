/*void build_vox_road(ColorRGBs* colors, VoxelNode *voctree, byte depth, color_rgb primary, byte unique_colors) {
    float2 mul_range = (float2) { 0.5f, 1.0f };
    for (int j = 0; j < unique_colors; j++) {
        color_rgb new_color = primary;
        float mul = randf_range(mul_range.x, mul_range.y);
        color_rgb_multiply_float(&new_color, mul);
        add_to_ColorRGBs(colors, new_color);
    }
    byte2 color_ids = (byte2) { colors->length + 1, colors->length + 1 + unique_colors };
    byte vlength = powers_of_two_byte[depth];
    byte3 size = byte3_single(vlength);
    byte3 pos;
    byte bark_thickness = vlength > 16 ? 2 : 1;
    byte ring_period = vlength / 6;
    if (ring_period < 2) ring_period = 2;
    sbyte cx = size.x / 2;
    sbyte cz = size.z / 2;
    for (pos.y = 0; pos.y < size.y; pos.y++) {
        for (pos.x = 0; pos.x < size.x; pos.x++) {
            for (pos.z = 0; pos.z < size.z; pos.z++) {
                // bark on XZ perimeter
                if (pos.x < bark_thickness || pos.x >= size.x - bark_thickness ||
                    pos.z < bark_thickness || pos.z >= size.z - bark_thickness) {
                    byte v = rand_range(color_ids.x, color_ids.y);
                    // vertical streaking
                    if ((pos.y + rand()) & 1) {
                        if (v > color_ids.x) {
                            v--;
                        }
                    }
                    set_VoxelNode(voctree, depth, pos, v);
                    continue;
                }
                // radial distance squared
                sbyte dx = (sbyte)pos.x - cx;
                sbyte dz = (sbyte)pos.z - cz;
                byte r2 = (byte)(dx*dx + dz*dz);
                // ring phase (density modulation, not hard bands)
                byte ring = (r2 / ring_period) & 1;
                // base wood color (always varied)
                byte v = rand_range(color_ids.x, color_ids.y);
                // ring density bias
                if (ring) {
                    if (v > color_ids.x) v--;
                } else {
                    if (v < color_ids.y) v++;
                }
                // vertical grain noise
                if ((pos.y + rand()) & 1) {
                    if (v > color_ids.x) v--;
                }
                set_VoxelNode(voctree, depth, pos, v);
            }
        }
    }
}*/

static inline uint32_t road_hash_u32(uint32_t x)
{
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

static inline uint32_t road_hash3(int x, int y, int z)
{
    uint32_t h = (uint32_t)x * 73856093U ^ (uint32_t)y * 19349663U ^ (uint32_t)z * 83492791U;
    return road_hash_u32(h);
}


// Positive modulo (always returns 0..b-1)
static inline int positive_mod(int a, int b)
{
    int m = a % b;
    if (m < 0) m += b;
    return m;
}

// NOTE: Fully supports negative coordinates (all sides work symmetrically).
//       Tiling, cracks and chips now look good on positive AND negative sides.
// NOTE: Tiled road/stone in all directions. Fully supports negative coordinates.

void build_vox_road2(ColorRGBs* colors, VoxelNode *voctree, byte depth, color_rgb primary, byte unique_colors) {
    if (unique_colors < 1) unique_colors = 1;

    byte first = colors->length + 1;
    byte last  = first + unique_colors - 1;
    byte crack = first;                    // darkest crack color
    byte line  = first + unique_colors / 2; // road line color (will be overridden)

    byte length = octree_size(depth);
    byte3 size = byte3_single(length);

    //==================================================================
    // EPIC COLOR RAMP - Asphalt, wear, lines, highlights (makes it POP)
    //==================================================================
    for (byte i = 0; i < unique_colors; i++)
    {
        color_rgb c = primary;
        float t = (unique_colors > 1) ? (float)i / (unique_colors - 1) : 0.0f;

        // Rich, cinematic asphalt feel
        float brightness = 0.48f + t * 1.45f;
        brightness += frand_range(-0.08f, 0.08f);

        // Slight hue shift toward cooler/warmer tones for depth
        if (i > unique_colors * 0.6f) {
            c.r = (byte)(c.r * 0.95f);
            c.b = (byte)(c.b * 1.08f);
        }

        color_rgb_multiply_float(&c, brightness);

        // Add subtle saturation boost on brighter tones
        if (t > 0.5f) {
            byte avg = (c.r + c.g + c.b) / 3;
            c.r = (byte)lerp(c.r, avg, 0.15f);
            c.g = (byte)lerp(c.g, avg, 0.15f);
            c.b = (byte)lerp(c.b, avg * 1.1f, 0.25f);
        }

        add_to_ColorRGBs(colors, c);
    }

    // Force a strong yellow-white dashed line color
    if (unique_colors >= 4) {
        color_rgb line_color = {255, 220, 80};
        color_rgb_multiply_float(&line_color, 0.92f);
        colors->value[first + unique_colors * 3 / 4] = line_color;
        line = first + unique_colors * 3 / 4;
    }

    int tile = length / 5;   // bigger tiles = bolder look
    if (tile < 6) tile = 6;

    byte3 pos;

    for (pos.x = 0; pos.x < size.x; pos.x++)
    for (pos.y = 0; pos.y < size.y; pos.y++)
    for (pos.z = 0; pos.z < size.z; pos.z++)
    {
        int tx = floor_div(pos.x, tile);
        int ty = floor_div(pos.y, tile);
        int tz = floor_div(pos.z, tile);

        int lx = positive_mod(pos.x, tile);
        int ly = positive_mod(pos.y, tile);
        int lz = positive_mod(pos.z, tile);

        uint32_t tile_hash = road_hash3(tx * 17 + tz, ty * 13, tx + tz * 23);

        byte color = first + (tile_hash % (unique_colors - 2)); // leave room for lines + cracks

        bool on_surface = (pos.x == 0 || pos.x == size.x-1) ||
                          (pos.y == 0 || pos.y == size.y-1) ||
                          (pos.z == 0 || pos.z == size.z-1);

        //==============================================================
        // ROAD LINES - the thing that makes it instantly viral
        //==============================================================
        bool is_line = false;
        if (on_surface) {
            int road_axis = (abs(tx) + abs(tz)) % 3; // variation
            if (road_axis == 0) { // longitudinal stripes
                if ((pos.x % (tile * 2)) < (tile / 2) && (pos.z % 7) < 3) {
                    is_line = true;
                }
            } else if (road_axis == 1) { // dashed across
                if ((pos.z % (tile * 2)) < (tile / 3)) {
                    is_line = true;
                }
            }
        }

        if (is_line) {
            color = line;
        }

        if (on_surface)
        {
            // Strong tile seams / expansion joints
            int edge_x = int_min(lx, tile - 1 - lx);
            int edge_y = int_min(ly, tile - 1 - ly);
            int edge_z = int_min(lz, tile - 1 - lz);

            if (edge_x <= 1 || edge_y <= 1 || edge_z <= 1)
            {
                uint32_t n = road_hash3(pos.x >> 1, pos.y >> 1, pos.z >> 1);
                if ((n & 15) < 11) {           // more cracks = dramatic
                    color = crack;
                }
            }

            // Organic surface cracks (very visible, high contrast)
            uint32_t crack_noise = road_hash3(pos.x * 5 + pos.z * 3, pos.y * 7 + pos.x, 0xDEADBEEF);
            if ((crack_noise & 127) == 0) {
                color = crack;
            }

            // Pothole / chipped areas - big dramatic ones
            int center = tile / 2;
            int manhattan = abs(lx - center) + abs(ly - center) + abs(lz - center);
            if (manhattan > tile * 1.35f) {
                if ((tile_hash & 31) < 7) {
                    color = crack;
                }
            }

            // Surface wear & speckling (gives that real road texture)
            if (color != crack && !is_line)
            {
                uint32_t wear = road_hash3(pos.x * 11 + pos.z * 7, pos.y * 13, 0xA55A);
                if ((wear & 63) == 0 && color < last - 1) {
                    color += 2; // brighter highlight speckles
                } else if ((wear & 31) == 1) {
                    color = (byte)int_max(first, color - 1); // subtle darkening
                }
            }

            // Occasional moss/grass edge (makes it feel alive)
            if ((pos.y == 0 || pos.y == size.y-1) && (road_hash3(pos.x, pos.z, 999) & 255) < 18) {
                color = first + (unique_colors * 4 / 5); // greenish tint if you have it
            }
        }
        else
        {
            // Interior - deeper, denser material
            if (color > first + 1)
                color -= 2;
        }

        // Final subtle random dirt
        if (!is_line && (road_hash3(pos.x, pos.y + 111, pos.z) & 511) == 0) {
            color = (byte)int_max(first, color - 1);
        }

        set_VoxelNode(voctree, depth, pos, color);
    }
}

void build_vox_cobblestone(ColorRGBs* colors, VoxelNode *voctree, byte depth, color_rgb primary)
{
    if (colors == NULL || voctree == NULL) return;
    byte unique_stone   = 8;
    byte unique_mortar  = 3;
    byte unique_wear    = 3;
    byte unique_grass   = 2;   // subtle moss / grass in cracks
    byte first = colors->length + 1;
    // ====================== COLOR PALETTE ======================
    // Stone variations (rich, slightly warm/cool gray-brown stones)
    for (byte i = 0; i < unique_stone; i++)
    {
        color_rgb c = primary;
        float t = (unique_stone > 1) ? (float)i / (unique_stone - 1) : 0.0f;
        float brightness = 0.65f + t * 0.75f;
        brightness += frand_range(-0.09f, 0.09f);
        color_rgb_multiply_float(&c, brightness);
        // Slight hue variation for natural stone feel
        if (i % 3 == 0)      c.r = (byte)(c.r * 1.08f);      // warmer
        else if (i % 3 == 1) c.b = (byte)(c.b * 1.06f);      // cooler
        add_to_ColorRGBs(colors, c);
    }
    byte stone_start = first;
    // Mortar (dark, slightly purple-gray)
    color_rgb mortar_base = {38, 35, 42};
    for (byte i = 0; i < unique_mortar; i++)
    {
        color_rgb c = mortar_base;
        color_rgb_multiply_float(&c, 0.85f + i * 0.12f);
        add_to_ColorRGBs(colors, c);
    }
    byte mortar_start = stone_start + unique_stone;
    // Worn / chipped stone highlights
    for (byte i = 0; i < unique_wear; i++)
    {
        color_rgb c = primary;
        color_rgb_multiply_float(&c, 1.15f + i * 0.08f);
        add_to_ColorRGBs(colors, c);
    }
    byte wear_start = mortar_start + unique_mortar;
    // Subtle moss/grass in cracks
    color_rgb moss_base = {42, 68, 35};
    for (byte i = 0; i < unique_grass; i++)
    {
        color_rgb c = moss_base;
        color_rgb_multiply_float(&c, 0.9f + i * 0.25f);
        add_to_ColorRGBs(colors, c);
    }
    byte moss_start = wear_start + unique_wear;
    // ====================== GEOMETRY ======================
    byte vlength = powers_of_two_byte[depth];
    byte3 size = byte3_single(vlength);
    byte3 pos;
    int tile = vlength / 6;          // cobblestone size
    if (tile < 5) tile = 5;
    //sbyte cx = size.x / 2;
    //sbyte cz = size.z / 2;
    for (pos.y = 0; pos.y < size.y; pos.y++)
    for (pos.x = 0; pos.x < size.x; pos.x++)
    for (pos.z = 0; pos.z < size.z; pos.z++)
    {
        // Base layer
        if (pos.y < vlength / 3)
        {
            byte v = stone_start + ((pos.x + pos.z * 7) % (unique_stone / 2));
            set_VoxelNode(voctree, depth, pos, v);
            continue;
        }
        int tx = floor_div(pos.x, tile);
        int tz = floor_div(pos.z, tile);
        int lx = positive_mod(pos.x, tile);
        int lz = positive_mod(pos.z, tile);
        uint32_t tile_hash = road_hash3(tx * 31 + tz * 37, tz * 17, tx ^ tz);
        // Determine if this voxel is inside a cobblestone or in mortar
        int stone_radius = (tile * 3) / 5;                    // irregular stone size
        int dx = lx - tile/2;
        int dz = lz - tile/2;
        int dist2 = dx*dx + dz*dz;
        byte is_mortar = dist2 > stone_radius * stone_radius +
            ((tile_hash & 7) - 3);   // organic variation
        byte color;
        if (is_mortar)
        {
            color = mortar_start + (tile_hash % unique_mortar);
            // Occasional moss in deep cracks
            if (((pos.x ^ pos.z) & 15) == 0 && (tile_hash & 31) < 12)
                color = moss_start + ((pos.x + pos.z) % unique_grass);
        }
        else
        {
            // Main stone
            color = stone_start + (tile_hash % unique_stone);

            // Surface wear / chips
            uint32_t wear = road_hash3(pos.x * 13 + pos.z * 11, pos.y, 0xC0B3);
            if ((wear & 63) == 0)
                color = wear_start + (wear % unique_wear);

            // Rounded top edge (gives classic cobblestone look)
            if (pos.y == vlength - 1)
            {
                if (dist2 > stone_radius * stone_radius * 9 / 10)
                    color = mortar_start;   // slight bevel
            }
        }
        // Subtle vertical dirt / aging
        if (pos.y < vlength - 2 && (road_hash3(pos.x, pos.y * 3, pos.z) & 127) == 0)
        {
            if (color >= stone_start && color < mortar_start)
                color = (byte)int_max(stone_start, color - 1);
        }
        set_VoxelNode(voctree, depth, pos, color);
    }
}

void build_vox_stoned(ColorRGBs *colors, VoxelNode *vox, byte depth, color_rgb primary) {
    const byte unique_colors = 9;
    const byte first = colors->length + 1;

    // Grey stone palette
    for (byte i = 0; i < unique_colors; i++) {
        float t = (float)i / (unique_colors - 1);

        color_rgb c = primary;

        float brightness = 0.60f + t * 0.55f;
        brightness += frand_range(-0.03f, 0.03f);

        color_rgb_multiply_float(&c, brightness);

        // Cool shadows
        c.b = byte_min(255, c.b + (byte)((1.0f - t) * 8));

        // Warm highlights
        c.r = byte_min(255, c.r + (byte)(t * 4));

        add_to_ColorRGBs(colors, c);
    }

    byte size = powers_of_two_byte[depth];

    byte3 pos;

    for (pos.x = 0; pos.x < size; pos.x++)
    for (pos.y = 0; pos.y < size; pos.y++)
    for (pos.z = 0; pos.z < size; pos.z++) {

        // Large stone blocks
        int bx = pos.x / 4;
        int by = pos.y / 4;
        int bz = pos.z / 4;

        uint h =
            bx * 73856093u ^
            by * 19349663u ^
            bz * 83492791u;

        h ^= h >> 13;
        h *= 1274126177u;

        float block = (h & 255) / 255.0f;

        // Small hand-cut texture
        float chip =
            sinf(pos.x * 1.6f) * 0.08f +
            cosf(pos.y * 1.9f) * 0.07f +
            sinf(pos.z * 1.4f) * 0.08f;

        // Horizontal quarry layers
        float strata =
            0.12f * sinf(pos.y * 0.45f);

        float f =
            block * 0.75f +
            chip +
            strata;

        if (f < 0.0f) f = 0.0f;
        if (f > 1.0f) f = 1.0f;

        byte color =
            first + (byte)(f * (unique_colors - 1));

        set_VoxelNode(vox, depth, pos, color);
    }
}

// NOTE: Generates a road block for towns
zox_sys2(RoadModelGenerationSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Seed);
    zox_sys_in(Color);
    zox_sys_in(VoxType);
    zox_sys_out(GenerateModel);
    zox_sys_out(VoxelNode);
    zox_sys_out(VoxelNodeDirty);
    zox_sys_out(NodeDepth);
    zox_sys_out(ColorRGBs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Seed, seed);
        zox_sys_i(Color, fill);
        zox_sys_i(VoxType, gentype);
        zox_sys_o(GenerateModel, generate);
        zox_sys_o(VoxelNode, node);
        zox_sys_o(VoxelNodeDirty, dirty);
        zox_sys_o(NodeDepth, depth);
        zox_sys_o(ColorRGBs, colors);
        if (generate->value != zox_generate_model_run) {
            continue;
        }
        if (gentype->value != vox_type_road) {
            continue;
        }
        // Reset Data
        resize_ColorRGBs(colors, 0);
        color_rgb primary_rgb = color_to_color_rgb(fill->value);
        // Generate Vox
        srand(seed->value);
        // write_lock_VoxelNode(node);
        build_vox_stoned(colors, node, depth->value, primary_rgb);
        // Outlines
        if (zox_block_outlines) {
            add_to_ColorRGBs(colors, color_rgb_black);
            byte black_voxel = colors->length;
            vox_outlines(node, depth->value, black_voxel);
        }
        // write_unlock_VoxelNode(node);
        generate->value = zox_has(e, BakeModel) ? zox_generate_model_bake : zox_generate_model_end;
        dirty->value = zox_dirty_trigger;
        if (dbg_log) {
            zox_log("Generated Road Vox [%s]:%i", zox_get_name(e), gentype->value);
        }
    }
} zox_sys_end(RoadModelGenerationSystem);
