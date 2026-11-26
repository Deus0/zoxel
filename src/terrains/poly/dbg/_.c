// --poly-map writes a shaded heightmap and exits at boot.
static byte poly_map_enabled = 0;
static lint poly_map_seed = 666;
static const char* poly_map_out = "poly";

void process_arguments_poly_terrains(ecs* world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "--poly-map") == 0) {
            poly_map_enabled = 1;
        } else if (strcmp(args[i], "--poly-seed") == 0 && i + 1 < count) {
            poly_map_seed = atol(args[++i]);
        } else if (strcmp(args[i], "--poly-out") == 0 && i + 1 < count) {
            poly_map_out = args[++i];
        }
    }
}

static byte poly_map_save(const char* path, const float* heights, int2 cells, float2 size) {
    FILE* file = fopen(path, "wb");
    if (!file) {
        return 0;
    }
    const int width = cells.x * 4;
    const int depth = cells.y * 4;
    const float3 light = poly_light_direction();
    const color_rgb water = { 40, 90, 170 };
    byte ok = fprintf(file, "P6\n%i %i\n255\n", width, depth) > 0;
    for (int z = 0; ok && z < depth; z++) {
        const float pz = (z + 0.5f) * size.y / depth;
        for (int x = 0; x < width; x++) {
            const float px = (x + 0.5f) * size.x / width;
            const float height = poly_height_at(heights, cells, size, px, pz);
            const float3 normal = poly_normal_at(heights, cells, size, px, pz);
            const color_rgb ground = poly_color_at(height, normal);
            const float shade = poly_light_at(normal, light);
            float red = shade * ground.r;
            float green = shade * ground.g;
            float blue = shade * ground.b;
            if (height < poly_terrain_sea) {
                red = float_lerp(water.r, red, 0.3f);
                green = float_lerp(water.g, green, 0.3f);
                blue = float_lerp(water.b, blue, 0.3f);
            }
            const byte pixel[] = { red, green, blue };
            if (fwrite(pixel, 1, sizeof(pixel), file) != sizeof(pixel)) {
                ok = 0;
                break;
            }
        }
    }
    return fclose(file) == 0 && ok;
}

void poly_map_on_boot(ecs* world, entity app) {
    (void) world;
    (void) app;
    if (!poly_map_enabled) {
        return;
    }
    const int2 cells = poly_terrain_cells;
    const float2 size = poly_terrain_size;
    float* heights = zox_malloc((size_t) (cells.x + 1) * (cells.y + 1) * sizeof(float));
    if (!heights) {
        zox_loge("[poly] couldn't allocate heights");
        engine_end();
        return;
    }
    poly_height_generate(heights, cells, size, poly_map_seed);
    char path[640];
    const int length = snprintf(path, sizeof(path), "%s/heights.ppm", poly_map_out);
    const byte saved = length >= 0 && length < sizeof(path)
        && zox_mkdirs(poly_map_out) && poly_map_save(path, heights, cells, size);
    zox_free(heights);
    if (saved) {
        zox_log("[poly] wrote [%s]", path);
    } else {
        zox_loge("[poly] couldn't write heights.ppm in [%s]", poly_map_out);
    }
    engine_end();
}
