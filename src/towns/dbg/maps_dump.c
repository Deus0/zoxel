// Maps dump (--maps): the maps of 3 x 3 regions as images, to see what world generation made (bsh/tools/maps.sh)
//  - a realm and terrain of its own from the menus (no game, no player), a stream point in the middle of region 0, 0,
//    streamed far enough to reach the regions round it (a circle: --far 68 reaches their corners)
//  - tunks only: their maps generate, no chunks (zox_terrain_no_chunks); --maps-chunks streams chunks too, for the
//    top blocks map
//  - its seed: --maps-seed <n>, else 666: the same seed draws the same maps
//  - waits for the area's tunks (and chunks), its regions to place their towns and the towns their roads, then writes
//    <out>/<map>.ppm (--maps-out <folder>, else maps), a pixel a block, x right and z down from the regions' low
//    corner, and quits; a tunk at a lod fills its cells' blocks (lod.ppm shows which):
//      height: the ground, dark low to light high, water blue over it
//      biome: a colour a biome; vegetation: dirt, grass, weeds, trees, flowers
//      town: its map (walls, gates, homes, roads, street lamps, highways); blocks: each column's top block's colour
//      lod: each tunk's lod, dark coarse to light full; overview: heights with towns and highways over them, region
//        borders white
#define maps_dump_regions 3
#define maps_dump_wait 600.0
#define maps_dump_count 7
byte maps_dump = 0;
byte maps_dump_chunks = 0;
lint maps_dump_seed = 666;
char maps_dump_out[512] = "maps";
byte maps_dump_stage = 0;
entity maps_dump_realm = 0;
entity maps_dump_terrain = 0;
double maps_dump_time = 0;
int maps_dump_frames = 0;
static const char* maps_dump_names[maps_dump_count] = { "height", "biome", "vegetation", "town", "blocks", "lod", "overview" };

static inline int maps_dump_region_blocks(ecs* world, entity terrain) {
    return octree_size(zox_getv(terrain, NodeDepth)) * region_dividor;
}

// counts the area's tunks done (and their chunks with --maps-chunks), 1 once they, its regions and towns are
static inline byte maps_dump_ready(ecs* world, entity terrain, int* done, int* total) {
    const int length = octree_size(zox_getv(terrain, NodeDepth));
    const int rb = maps_dump_region_blocks(world, terrain);
    const TunkLinks* tunks = zox_get(terrain, TunkLinks);
    *done = 0;
    *total = 0;
    for (int z = -rb; z < rb * 2; z += length)
    for (int x = -rb; x < rb * 2; x += length) {
        const entity tunk = int2_hashmap_get(tunks->value, (int2) { floor_div(x, length), floor_div(z, length) });
        (*total)++;
        *done += zox_valid(tunk) && !zox_getv(tunk, GenerateTunk) && (!maps_dump_chunks || tunk_map_colors(world, tunk));
    }
    const RegionLinks* regions = zox_get(terrain, RegionLinks);
    for (int y = -1; y <= 1; y++)
    for (int x = -1; x <= 1; x++) {
        const entity region = int2_hashmap_get(regions->value, (int2) { x, y });
        if (!zox_valid(region) || zox_getv(region, GenerateRegion)) {
            return 0;
        }
        entity towns[zox_children_capacity];
        const uint count = zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town));
        for (uint t = 0; t < count; t++) {
            if (zox_getv(towns[t], GenerateTown)) {
                return 0;
            }
        }
    }
    return *done == *total;
}

static inline void maps_dump_set(byte* image, int side, int x, int y, color c) {
    if (x < 0 || y < 0 || x >= side || y >= side) {
        return;
    }
    byte* p = &image[((size_t) y * side + x) * 3];
    p[0] = c.r;
    p[1] = c.g;
    p[2] = c.b;
}

static inline color maps_dump_town_color(byte value) {
    switch (value) {
        case zox_town_type_zone: return (color) { 70, 64, 56, 255 };
        case zox_town_type_wall: return (color) { 200, 200, 200, 255 };
        case zox_town_type_wall_edge: return (color) { 225, 225, 225, 255 };
        case zox_town_type_wall_spike: return (color) { 250, 250, 250, 255 };
        case zox_town_type_gate: return (color) { 220, 80, 80, 255 };
        case zox_town_type_home: return (color) { 170, 120, 80, 255 };
        case zox_town_type_road: return (color) { 150, 140, 170, 255 };
        case zox_town_type_lamp: return (color) { 255, 230, 90, 255 };
        case zox_town_type_highway: return (color) { 240, 150, 50, 255 };
        default: return (color) { 16, 16, 16, 255 };
    }
}

static inline color maps_dump_vegetation_color(byte value) {
    switch (value) {
        case zox_vegetation_dirt: return (color) { 110, 80, 50, 255 };
        case zox_vegetation_grass: return (color) { 90, 170, 70, 255 };
        case zox_vegetation_weeds: return (color) { 140, 150, 60, 255 };
        case zox_vegetation_trees: return (color) { 30, 90, 40, 255 };
        case zox_vegetation_flowers: return (color) { 230, 120, 180, 255 };
        default: return (color) { 128, 128, 128, 255 };
    }
}

// the ground's colour: dark low to light high, blue under the sea
static inline color maps_dump_height_color(int height, int sea, int max_height) {
    const byte v = (byte) int_clamp(40 + height * 215 / int_max(max_height, 1), 0, 255);
    return height < sea ? (color) { v / 4, v / 3, (byte) int_min(v + 60, 255), 255 } : (color) { v, v, v, 255 };
}

static inline byte maps_dump_save(const char* name, const byte* pixels, int side) {
    char path[640];
    snprintf(path, sizeof(path), "%s/%s.ppm", maps_dump_out, name);
    FILE* file = fopen(path, "wb");
    if (!file) {
        zox_loge("[maps] couldn't write [%s]", path);
        return 0;
    }
    fprintf(file, "P6\n%i %i\n255\n", side, side);
    fwrite(pixels, 1, (size_t) side * side * 3, file);
    fclose(file);
    return 1;
}

void maps_dump_write(ecs* world, entity terrain) {
    const int length = octree_size(zox_getv(terrain, NodeDepth));
    const int rb = maps_dump_region_blocks(world, terrain);
    const int side = rb * maps_dump_regions;
    const int sea = terrain_sea_height * render_distance_y;
    const int max_height = int_min(render_distance_y * (length - 1), 255);
    byte* images[maps_dump_count];
    for (byte m = 0; m < maps_dump_count; m++) {
        images[m] = calloc((size_t) side * side * 3, 1);
    }
    const TunkLinks* tunks = zox_get(terrain, TunkLinks);
    int drawn = 0, missing = 0;
    for (int tz = 0; tz < side / length; tz++)
    for (int tx = 0; tx < side / length; tx++) {
        const entity tunk = int2_hashmap_get(tunks->value, (int2) { floor_div(-rb, length) + tx, floor_div(-rb, length) + tz });
        if (!zox_valid(tunk) || zox_getv(tunk, GenerateTunk)) {
            missing++;
            continue;
        }
        drawn++;
        const byte lod = zox_getv(tunk, TunkLod);
        const int cells = octree_size(lod), cell_blocks = length / cells;
        const HeightMap* heights = zox_get(tunk, HeightMap);
        const BiomeMap* biomes = zox_get(tunk, BiomeMap);
        const VegetationMap* vegetation = zox_get(tunk, VegetationMap);
        const TownMap* towns = zox_get(tunk, TownMap);
        const byte lod_v = (byte) int_clamp(30 + lod * 225 / int_max(zox_getv(terrain, NodeDepth), 1), 0, 255);
        for (int my = 0; my < cells; my++)
        for (int mx = 0; mx < cells; mx++) {
            const int index = mx + my * cells;
            const int height = index < heights->length ? heights->value[index] : 0;
            const byte biome = index < biomes->length ? biomes->value[index] : 0;
            const byte plant = index < vegetation->length ? vegetation->value[index] : 255;
            const byte town = index < towns->length ? towns->value[index] : 0;
            const color height_color = maps_dump_height_color(height, sea, max_height);
            const color biome_color = color_rgb_to_color(hsv_to_color_rgb((float3) { (biome * 67) % 360, 55, 75 }));
            for (int by = 0; by < cell_blocks; by++)
            for (int bx = 0; bx < cell_blocks; bx++) {
                const int px = tx * length + mx * cell_blocks + bx, py = tz * length + my * cell_blocks + by;
                maps_dump_set(images[0], side, px, py, height_color);
                maps_dump_set(images[1], side, px, py, biome_color);
                maps_dump_set(images[2], side, px, py, maps_dump_vegetation_color(plant));
                maps_dump_set(images[3], side, px, py, maps_dump_town_color(town));
                maps_dump_set(images[5], side, px, py, (color) { lod_v, lod_v, lod_v, 255 });
                maps_dump_set(images[6], side, px, py, town ? maps_dump_town_color(town) : height_color);
            }
        }
        // its top blocks, at its chunks' detail (--maps-chunks)
        const TunkMapColors* colors = maps_dump_chunks ? zox_get(tunk, TunkMapColors) : NULL;
        const int colors_side = colors && colors->length ? (int) roundf(sqrtf(colors->length)) : 0;
        for (int cy = 0; cy < colors_side; cy++)
        for (int cx = 0; cx < colors_side; cx++) {
            const int each = length / colors_side;
            for (int by = 0; by < each; by++)
            for (int bx = 0; bx < each; bx++) {
                maps_dump_set(images[4], side, tx * length + cx * each + bx, tz * length + cy * each + by,
                    colors->value[cx + cy * colors_side]);
            }
        }
    }
    // region borders on the overview
    for (int k = 0; k <= maps_dump_regions; k++) {
        for (int i = 0; i < side; i++) {
            maps_dump_set(images[6], side, k * rb, i, color_white);
            maps_dump_set(images[6], side, i, k * rb, color_white);
        }
    }
    zox_mkdirs(maps_dump_out);
    for (byte m = 0; m < maps_dump_count; m++) {
        if (m != 4 || maps_dump_chunks) {
            maps_dump_save(maps_dump_names[m], images[m], side);
        }
        free(images[m]);
    }
    zox_log("[maps] [%i x %i] blocks from [%i %i], seed [%li], [%i] tunks drawn, [%i] missing, in [%s]",
        side, side, -rb, -rb, (long) maps_dump_seed, drawn, missing, maps_dump_out);
}

void maps_dump_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const double now = get_time_ms() / 1000.0;
    maps_dump_frames++;
    if (maps_dump_stage == 0) {
        // a realm of its own, once the menus are up
        if (!zox_valid(local_game) || !zox_valid(main_app) || maps_dump_frames < 5) {
            return;
        }
        maps_dump_realm = spawn_realm(world, local_game, maps_dump_seed);
        zox_setv(maps_dump_realm, GenerateRealm, zox_generate_realm_start);
        maps_dump_stage = 1;
        return;
    }
    if (maps_dump_stage == 1) {
        if (zox_getv(maps_dump_realm, GenerateRealm)) {
            return;
        }
        maps_dump_terrain = spawn_terrain(world, maps_dump_realm, zox_getv(maps_dump_realm, Seed));
        maps_dump_stage = 2;
        return;
    }
    const entity terrain = maps_dump_terrain;
    if (maps_dump_stage == 2) {
        // a stream point in the middle of region 0, 0
        const float scale = zox_getv(terrain, BlockScale);
        const float middle = maps_dump_region_blocks(world, terrain) * 0.5f * scale;
        const float chunk = octree_size(zox_getv(terrain, NodeDepth)) * scale;
        const entity streamer = spawn_streamer(world, prefab_streamer, terrain, (float3) { middle, chunk, middle });
        zox_setv(streamer, StreamerLevel, 1);
        maps_dump_time = now;
        maps_dump_stage = 3;
        zox_log("[maps] seed [%li]: streaming 3 x 3 regions round [%.0f %.0f]%s", (long) maps_dump_seed,
            middle / scale, middle / scale, maps_dump_chunks ? " with chunks" : "");
        return;
    }
    // a look every 30 frames (a walk over ~9000 tunks)
    if (maps_dump_frames % 30) {
        return;
    }
    int done, total;
    const byte ready = maps_dump_ready(world, terrain, &done, &total);
    if (maps_dump_frames % 300 == 0) {
        zox_log("[maps] [%i of %i] tunks done after [%.0fs]", done, total, now - maps_dump_time);
    }
    if (!ready && now - maps_dump_time < maps_dump_wait) {
        return;
    }
    if (!ready) {
        zox_logw("[maps] still generating after [%.0fs] ([%i of %i] tunks), drawing what's there", now - maps_dump_time, done, total);
    }
    maps_dump_write(world, terrain);
    engine_end();
    zox_sys_on_end();
} zoxd_system(maps_dump_system);

void process_arguments_maps_dump(ecs* world, char* args[], int count) {
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "--maps") == 0) {
            maps_dump = 1;
        } else if (strcmp(args[i], "--maps-chunks") == 0) {
            maps_dump = 1;
            maps_dump_chunks = 1;
        } else if (strcmp(args[i], "--maps-seed") == 0 && i + 1 < count) {
            maps_dump_seed = atol(args[++i]);
        } else if (strcmp(args[i], "--maps-out") == 0 && i + 1 < count) {
            snprintf(maps_dump_out, sizeof(maps_dump_out), "%s", args[++i]);
        }
    }
    if (maps_dump) {
        zox_terrain_no_chunks = !maps_dump_chunks;
        zox_enable(zox_id(maps_dump_system));
    }
}
