void build_vox_road(ColorRGBs* colors, VoxelNode *voctree, byte depth, color_rgb primary) {
    // === Rich, varied palette ===
    byte unique_asphalt = 6;
    byte unique_line = 3;
    byte unique_curb = 3;
    byte unique_grass = 4;
    byte unique_crack = 3;
    byte unique_glow = 2;
    float2 mul_range = (float2){0.45f, 1.15f};
    // Asphalt
    color_rgb asphalt_base = primary;
    for (int i = 0; i < unique_asphalt; i++) {
        color_rgb c = asphalt_base;
        color_rgb_multiply_float(&c, randf_range(mul_range.x, mul_range.y));
        c.r = (byte)(c.r * 0.92f);
        c.g = (byte)(c.g * 0.88f);
        c.b = (byte)(c.b * 1.08f);
        add_to_ColorRGBs(colors, c);
    }
    // Bright road lines
    color_rgb line_base = (color_rgb){255, 215, 50};
    for (int i = 0; i < unique_line; i++) {
        color_rgb c = line_base;
        color_rgb_multiply_float(&c, randf_range(0.9f, 1.25f));
        add_to_ColorRGBs(colors, c);
    }
    // Concrete curbs
    color_rgb curb_base = (color_rgb){195, 195, 205};
    for (int i = 0; i < unique_curb; i++) {
        color_rgb c = curb_base;
        color_rgb_multiply_float(&c, randf_range(0.75f, 1.25f));
        add_to_ColorRGBs(colors, c);
    }
    // Overgrown grass shoulders
    color_rgb grass_base = (color_rgb){35, 125, 45};
    for (int i = 0; i < unique_grass; i++) {
        color_rgb c = grass_base;
        color_rgb_multiply_float(&c, randf_range(0.65f, 1.45f));
        if (rand() % 2) c.b += 25;
        add_to_ColorRGBs(colors, c);
    }
    // Cracks
    color_rgb crack_base = (color_rgb){28, 22, 32};
    for (int i = 0; i < unique_crack; i++) {
        color_rgb c = crack_base;
        color_rgb_multiply_float(&c, randf_range(0.6f, 1.0f));
        add_to_ColorRGBs(colors, c);
    }

    // Neon glow accents
    color_rgb glow_base = (color_rgb){60, 255, 220};
    for (int i = 0; i < unique_glow; i++) {
        color_rgb c = glow_base;
        color_rgb_multiply_float(&c, randf_range(0.85f, 1.4f));
        add_to_ColorRGBs(colors, c);
    }
    byte asphalt_start = colors->length - (unique_asphalt + unique_line + unique_curb + unique_grass + unique_crack + unique_glow);
    byte line_start   = asphalt_start + unique_asphalt;
    byte curb_start   = line_start + unique_line;
    byte grass_start  = curb_start + unique_curb;
    byte crack_start  = grass_start + unique_grass;
    byte glow_start   = crack_start + unique_crack;
    short vlength = octree_size(depth);
    byte3 size = byte3_single(vlength);
    byte3 pos;

    sbyte cx = size.x / 2;
    sbyte road_half_width = vlength / 5;
    sbyte curb_w = vlength > 16 ? 2 : 1;
    sbyte shoulder_w = vlength / 7;
    // Tileable dash period (works across multiple segments)
    byte dash_period = (vlength >= 32) ? 12 : 8;
    if (dash_period < 4) dash_period = 4;

    for (pos.y = 0; pos.y < size.y; pos.y++) {
        for (pos.x = 0; pos.x < size.x; pos.x++) {
            for (pos.z = 0; pos.z < size.z; pos.z++) {
                sbyte dx = (sbyte)pos.x - cx;
                byte abs_dx = dx < 0 ? -dx : dx;

                // Deterministic height variation (tileable)
                byte height_noise = ((pos.x * 13 + pos.z * 7) % 7);
                byte road_top = (byte)(vlength * 0.36f) + (height_noise > 3 ? 1 : 0);

                if (pos.y >= road_top + 4) continue;   // sky

                // === Grass shoulders (tileable noise) ===
                if (abs_dx > road_half_width + curb_w + shoulder_w) {
                    if (pos.y < road_top + ((pos.x + pos.z) % 3)) {
                        byte grass_idx = ((pos.x * 17 + pos.z * 11 + pos.y * 5) % unique_grass);
                        set_voxel_safe(voctree, depth, pos, grass_start + grass_idx);
                    }
                    continue;
                }

                // === Curbs ===
                if (abs_dx > road_half_width && abs_dx <= road_half_width + curb_w) {
                    byte v = curb_start + ((pos.x + pos.z) % unique_curb);
                    // weathered edge
                    if (((pos.x + pos.z + pos.y) % 11) == 0) {
                        v = crack_start + (pos.y % unique_crack);
                    }
                    set_voxel_safe(voctree, depth, pos, v);
                    continue;
                }

                // === Main road ===
                if (abs_dx <= road_half_width) {
                    byte v = asphalt_start + ((pos.x * 19 + pos.z * 23 + pos.y) % unique_asphalt);

                    // Cracks (seamless)
                    int crack_seed = pos.x * 31 + pos.z * 37 + pos.y * 3;
                    if ((crack_seed % 29) < 5 || ((pos.x ^ pos.z) & 15) == 0) {
                        v = crack_start + (crack_seed % unique_crack);
                    }

                    // Center glowing dashed line - perfectly tileable
                    if (abs_dx <= 1) {
                        byte dash_phase = (pos.z % dash_period);
                        if (dash_phase < (dash_period * 2 / 5)) {  // solid dash part
                            if (pos.y == road_top - 1) {
                                v = glow_start + (pos.z % unique_glow);
                            }
                        } else if (pos.y == road_top - 1) {
                            v = line_start + (pos.z % unique_line);
                        }
                    }
                    // Side lines
                    else if (abs_dx == road_half_width - 1 && pos.y == road_top - 1) {
                        if ((pos.z % 6) < 4) {
                            v = line_start + (pos.z % unique_line);
                        }
                    }

                    // Surface wear texture
                    if (((pos.x + pos.z * 3) % 5) == 0 && pos.y == road_top - 1) {
                        v = asphalt_start + ((v - asphalt_start + 1) % unique_asphalt);
                    }
                    set_voxel_safe(voctree, depth, pos, v);
                    continue;
                }

                // Shoulders
                if (pos.y < road_top) {
                    byte v = asphalt_start + ((pos.x + pos.z * 5) % (unique_asphalt / 2 + 1));
                    set_voxel_safe(voctree, depth, pos, v);
                }
            }
        }
    }
}