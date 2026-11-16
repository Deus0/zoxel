// tst: water_block, the realm's water: see through, walked through, its texture moves
//  - the realm has a water block: a liquid material, an air collider, light passing
//  - its tilemap tile animates: the tilemap's pixels there change over time
//  - placed in an all air chunk: two water blocks side by side and bricks at one end
//      water draws a blended mesh (TransparentMesh) with only its outside faces: not between the two waters, nor
//      against the bricks (9 faces); the bricks keep their face against the water (6 faces)
//  - run: bash bsh/tst.sh water_block
byte zox_tst_water_stage = 0;
double zox_tst_water_time = 0;
entity zox_tst_water_chunk = 0;
uint zox_tst_water_pixels = 0;

// a tilemap tile's pixels summed, to see it change
static inline uint zox_tst_water_tile(ecs* world, entity tilemap, int2 position, int2 size) {
    const TextureData* data = zox_get(tilemap, TextureData);
    const int2 map_size = zox_getv(tilemap, TextureSize);
    uint sum = 0;
    for (int y = 0; y < size.y; y++) for (int x = 0; x < size.x; x++) {
        const color c = data->value[(position.y + y) * map_size.x + position.x + x];
        sum += c.r * 3 + c.g * 5 + c.b * 7 + (x + y * size.x) * (c.r + c.g + c.b);
    }
    return sum;
}

// the realm's tilemap's water tile: its animation, 0 if none
static inline const TilemapAnimation* zox_tst_water_animation(ecs* world, entity tilemap) {
    if (!zox_valid(tilemap) || !zox_has(tilemap, TilemapAnimations)) {
        return NULL;
    }
    const TilemapAnimations* animations = zox_get(tilemap, TilemapAnimations);
    for (int i = 0; i < animations->length; i++) {
        if (zox_valid(animations->value[i].texture) && zox_has(animations->value[i].texture, WaterTexture)) {
            return &animations->value[i];
        }
    }
    return NULL;
}

// a chunk's shown meshes' vertices, the see through ones and the rest
static inline void zox_tst_water_vertices(ecs* world, entity chunk, int* water, int* solid, int* water_meshes) {
    entity meshes[zox_chunk_meshes_max];
    const uint count = get_chunk_meshes(world, chunk, meshes);
    *water = *solid = *water_meshes = 0;
    for (uint i = 0; i < count; i++) {
        if (zox_has(meshes[i], Disabled)) {
            continue;
        }
        const int vertices = zox_has(meshes[i], MeshVertices) ? zox_get(meshes[i], MeshVertices)->length : 0;
        if (zox_has(meshes[i], TransparentMesh)) {
            *water += vertices;
            *water_meshes += 1;
        } else {
            *solid += vertices;
        }
    }
}

byte zox_tst_water_block_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    zox_tst_progress(zox_tst_water_stage * 1000 + (int) (zox_current_time - zox_tst_water_time));
    if (zox_tst_water_stage == 0) {
        zox_tst_waiting = "an all air chunk over the player";
        zox_tst_water_chunk = zox_tst_air_chunk(world, character, terrain);
        if (!zox_tst_water_chunk) {
            return 0;
        }
        const entity water = zox_get_child_by_id(world, realm, zox_id(BlockWater));
        const entity bricks = zox_get_child_by_id(world, realm, zox_id(BlockBricks));
        zox_tst_check(zox_valid(water) && zox_valid(bricks), "the realm has no water or bricks block");
        if (!zox_valid(water) || !zox_valid(bricks)) {
            return 1;
        }
        VoxelNodeQueue* queue = zox_getm(zox_tst_water_chunk, VoxelNodeQueue);
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = zox_getv(water, BlockIndex), .position = (byte3) { 4, 8, 8 } });
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = zox_getv(water, BlockIndex), .position = (byte3) { 5, 8, 8 } });
        a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = zox_getv(bricks, BlockIndex), .position = (byte3) { 6, 8, 8 } });
        const entity tilemap = zox_get_link(world, zox_tst_water_chunk, Tilemap);
        const TilemapAnimation* animation = zox_tst_water_animation(world, tilemap);
        if (animation) {
            zox_tst_water_pixels = zox_tst_water_tile(world, tilemap, animation->position, zox_getv(animation->texture, TextureSize));
        }
        zox_tst_water_stage = 1;
        zox_tst_water_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "the water's and bricks' meshes";
    int water, solid, water_meshes;
    zox_tst_water_vertices(world, zox_tst_water_chunk, &water, &solid, &water_meshes);
    if (water && solid && !zox_has(zox_tst_water_chunk, ChunkUpdate)) {
        return zox_current_time - zox_tst_water_time > 1.5;
    }
    return zox_current_time - zox_tst_water_time > 15;
}

void zox_tst_water_block_check(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    const entity water = zox_get_child_by_id(world, realm, zox_id(BlockWater));
    zox_tst_check(zox_valid(water), "the realm has no water block");
    if (!zox_valid(water) || !zox_valid(zox_tst_water_chunk)) {
        return;
    }
    zox_tst_check(zox_getv(water, BlockMaterial) == zox_block_material_liquid, "water isn't a liquid material");
    zox_tst_check(zox_getv(water, BlockCollider) == zox_block_air, "water isn't walked through");
    zox_tst_check(zox_getv(water, BlockLightPass), "water stops light");
    // its tile moves
    const entity tilemap = zox_get_link(world, zox_tst_water_chunk, Tilemap);
    const TilemapAnimation* animation = zox_tst_water_animation(world, tilemap);
    zox_tst_check(animation != NULL, "the tilemap has no water animation");
    if (animation) {
        const uint now = zox_tst_water_tile(world, tilemap, animation->position, zox_getv(animation->texture, TextureSize));
        const TextureFrames* frames = zox_get(animation->texture, TextureFrames);
        const int2 size = zox_getv(animation->texture, TextureSize);
        zox_log("[tst] [metric] water_block tile at [%ix%i]: [%i] frames, frame now [%i], pixels [%u] then [%u]", animation->position.x,
            animation->position.y, frames->length / (size.x * size.y), animation->frame, zox_tst_water_pixels, now);
        zox_tst_check(frames->length / (size.x * size.y) == zox_water_frames, "water has [%i] frames, not [%i]", frames->length / (size.x * size.y), zox_water_frames);
        zox_tst_check(now != zox_tst_water_pixels, "the water tile didn't change in [%.1fs]", zox_current_time - zox_tst_water_time);
        // it fills its tile: a tile is the tilemap's size over its tiles a side, none of it left blank (white)
        const int2 tiles = zox_getv(tilemap, TilemapSize), map_size = zox_getv(tilemap, TextureSize);
        const int2 tile = { map_size.x / tiles.x, map_size.y / tiles.y };
        const TextureData* data = zox_get(tilemap, TextureData);
        int blank = 0;
        for (int y = 0; y < tile.y; y++) for (int x = 0; x < tile.x; x++) {
            const color c = data->value[(animation->position.y + y) * map_size.x + animation->position.x + x];
            blank += c.r == 255 && c.g == 255 && c.b == 255;
        }
        zox_log("[tst] [metric] water_block its tile [%ix%i], its texture [%ix%i], [%i] blank pixels in the tile", tile.x, tile.y, size.x, size.y, blank);
        zox_tst_check(!blank, "[%i] of the water tile's [%i] pixels are blank: its texture doesn't fill its tile", blank, tile.x * tile.y);
        zox_tst_check(size.x == tile.x && size.y == tile.y && size.x == powers_of_two[block_depth],
            "the water texture is [%ix%i], its tile [%ix%i], a block's texture [%i]: it would show at a part of its face", size.x, size.y,
            tile.x, tile.y, powers_of_two[block_depth]);
    }
    // its faces
    int vertices, solid, water_meshes;
    zox_tst_water_vertices(world, zox_tst_water_chunk, &vertices, &solid, &water_meshes);
    zox_log("[tst] [metric] water_block meshes: water [%i] ([%i] faces, [%i] meshes), bricks [%i] ([%i] faces)", vertices, vertices / 4,
        water_meshes, solid, solid / 4);
    // its faces' uvs span its tile: a face's uvs across a tile's share of the tilemap each way
    {
        entity meshes[zox_chunk_meshes_max];
        const uint count = get_chunk_meshes(world, zox_tst_water_chunk, meshes);
        const entity tilemap = zox_get_link(world, zox_tst_water_chunk, Tilemap);
        const int2 tiles = zox_getv(tilemap, TilemapSize);
        for (uint i = 0; i < count; i++) {
            if (zox_has(meshes[i], Disabled) || !zox_has(meshes[i], TransparentMesh) || !zox_has(meshes[i], MeshUVs)) {
                continue;
            }
            const MeshUVs* uvs = zox_get(meshes[i], MeshUVs);
            for (int f = 0; f + 3 < uvs->length && f < 4 * 9; f += 4) {
                float2 lo = uvs->value[f], hi = uvs->value[f];
                for (int k = 1; k < 4; k++) {
                    const float2 u = uvs->value[f + k];
                    lo = (float2) { u.x < lo.x ? u.x : lo.x, u.y < lo.y ? u.y : lo.y };
                    hi = (float2) { u.x > hi.x ? u.x : hi.x, u.y > hi.y ? u.y : hi.y };
                }
                const float2 span = { (hi.x - lo.x) * tiles.x, (hi.y - lo.y) * tiles.y };
                if (f == 0) {
                    zox_log("[tst] [metric] water_block a face's uvs span [%.3f x %.3f] tiles (tilemap [%ix%i] tiles)", span.x, span.y, tiles.x, tiles.y);
                }
                zox_tst_check(fabsf(span.x - 1) < 0.01f && fabsf(span.y - 1) < 0.01f, "a water face's uvs span [%.3f x %.3f] of its tile, not all of it",
                    span.x, span.y);
            }
        }
    }
    zox_tst_check(water_meshes == 1, "[%i] see through meshes shown, not one", water_meshes);
    zox_tst_check(vertices == 9 * 4, "the water has [%i] faces, not its 9 outside ones (none between its blocks or against the bricks)", vertices / 4);
    zox_tst_check(solid == 6 * 4, "the bricks have [%i] faces, not all 6 (its face against the water shows)", solid / 4);
}

// tst: sea_level, ground below the sea is under water up to it (heights/sys/landfill.c terrain_sea_height)
//  - no player: a realm and terrain of its own (world_only.c), one camera over the nearest open water to the
//      origin, streaming the land round it, looking down at it, drawn alone
//  - every generated chunk at full detail round there, against its tunk's heights: a column whose ground is
//      below the sea holds water from just over its ground up to the sea, and nothing over the sea is water
//  - screenshots it twice half a second apart (tst/sea.ppm, sea_later.ppm):
//      the view's middle is mostly water, and it moves
//  - logs how much of the land is at or below each height (over 2048 blocks), to set the sea's level by
//  - run: bash bsh/tst.sh sea_level
int zox_tst_sea_town_air = 0, zox_tst_sea_lowest = 255, zox_tst_sea_columns = 0, zox_tst_sea_wrong = 0, zox_tst_sea_above = 0, zox_tst_sea_chunks = 0;

double zox_tst_sea_begin = 0, zox_tst_sea_streamed = 0;
float3 zox_tst_sea_spot;
byte zox_tst_sea_found = 0;
entity zox_tst_sea_camera = 0;

// the realm's water block's index, 0 if none
static inline byte zox_tst_sea_water_id(ecs* world, entity terrain) {
    const entity water = zox_get_child_by_id(world, zox_get_parent(world, terrain), zox_id(BlockWater));
    return zox_valid(water) ? zox_getv(water, BlockIndex) : 0;
}

// a camera of its own (no player): over the nearest open water, looking down at it, streaming the land round it
byte zox_tst_sea_level_ready(ecs* world) {
    // no player: a realm and terrain of its own (world_only.c), streamed only round this camera
    const entity terrain = zox_tst_world_ready(world);
    TerrainHeights th;
    if (!terrain || !terrain_heights_begin(world, terrain, &th)) {
        return 0;
    }
    const float scale = zox_getv(terrain, BlockScale);
    const int sea = terrain_sea_height * render_distance_y;
    if (!zox_tst_sea_begin) {
        zox_tst_sea_begin = zox_current_time;
        // the nearest open water to the origin, from the terrain's heights: the spot and 4 blocks round it under the
        //  sea (so it fills the view), no town there
        for (int r = 0; r <= 600 && sea && !zox_tst_sea_found; r += 8)
        for (int x = -r; x <= r && !zox_tst_sea_found; x += 8)
        for (int z = -r; z <= r && !zox_tst_sea_found; z += 8) {
            if (abs(x) != r && abs(z) != r) {
                continue;
            }
            byte open = 1;
            for (int k = 0; k < 5 && open; k++) {
                const int2 o = k == 0 ? int2_zero : (int2) { k == 1 ? 4 : k == 2 ? -4 : 0, k == 3 ? 4 : k == 4 ? -4 : 0 };
                open = terrain_height_at(&th, (int2) { x + o.x, z + o.y }) < sea - 1;
            }
            if (!open || town_at(world, (int2) { x, z })) {
                continue;
            }
            zox_tst_sea_found = 1;
            zox_tst_sea_spot = (float3) { (x + 0.5f) * scale, (sea + 1) * scale, (z + 0.5f) * scale };
        }
        // no old screenshots read as this run's
        remove(zox_tst_path("sea.ppm"));
        remove(zox_tst_path("sea_later.ppm"));
        zox_log("[tst] sea_level: open water [%s] at block [%.0fx%.0f]", zox_tst_sea_found ? "found" : "not found",
            zox_tst_sea_spot.x / scale, zox_tst_sea_spot.z / scale);
        if (!zox_tst_sea_found) {
            return 1;
        }
        // 8 blocks up and 5 back, looking down at it; drawn alone on the screen (no hud, no game view)
        // zox_tst_sea_near: 2 blocks up and 1 back, to see a block's texture close
        const byte near = getenv("zox_tst_sea_near") != NULL;
        const float3 eye = float3_add(zox_tst_sea_spot, near ? (float3) { 0, 2 * scale, 1 * scale } : (float3) { 0, 8 * scale, 5 * scale });
        zox_tst_sea_camera = spawn_viewport_camera(world, main_app, eye, camera_look_at(eye, zox_tst_sea_spot), 70,
            viewport_grid_cell(0, 1, 1));
        prefab_add_streamer(world, zox_tst_sea_camera);
        zox_set_link(world, zox_tst_sea_camera, StreamLink, terrain);
        // 1 streams the land itself (0 regions and towns only)
        zox_setv(zox_tst_sea_camera, StreamerLevel, 1);
        nohud = 1;
        const entity game_view = zox_valid(main_cameras[0]) ? zox_get_link(world, main_cameras[0], RenderTextureLink) : 0;
        if (zox_valid(game_view)) {
            zox_setv(game_view, RenderDisabled, 1);
        }
    }
    zox_tst_progress((int) (zox_current_time - zox_tst_sea_begin));
    // until the chunk at the sea's level there shows a water mesh, then a while for its neighbours and lights
    zox_tst_waiting = "the land under the camera to stream in and show its water";
    const int3 chunk_position = real_position_to_chunk_position(zox_tst_sea_spot, octree_size(zox_getv(terrain, NodeDepth)), scale);
    const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, chunk_position);
    if (!zox_tst_sea_streamed) {
        int water = 0, solid = 0, meshes = 0;
        if (zox_valid(chunk)) {
            zox_tst_water_vertices(world, chunk, &water, &solid, &meshes);
        }
        if (!water) {
            if (zox_current_time - zox_tst_sea_begin > 60) {
                entity meshes[zox_chunk_meshes_max];
                const uint count = zox_valid(chunk) ? get_chunk_meshes(world, chunk, meshes) : 0;
                zox_log("[tst] sea_level: chunk [%ix%ix%i] [%s] depth [%i] generate [%i] meshes [%i] stream [%ix%ix%i]",
                    chunk_position.x, chunk_position.y, chunk_position.z, zox_valid(chunk) ? zox_getn(chunk) : "none",
                    zox_valid(chunk) ? zox_getv(chunk, NodeDepth) : -1, zox_valid(chunk) && zox_has(chunk, GenerateChunk) ? zox_getv(chunk, GenerateChunk) : -1,
                    count, zox_getv(zox_tst_sea_camera, StreamPosition).x, zox_getv(zox_tst_sea_camera, StreamPosition).y, zox_getv(zox_tst_sea_camera, StreamPosition).z);
                for (uint i = 0; i < count; i++) {
                    zox_log("[tst]   mesh [%s] disabled [%i] transparent [%i] vertices [%i] depth [%i]", zox_getn(meshes[i]), zox_has(meshes[i], Disabled),
                        zox_has(meshes[i], TransparentMesh), zox_has(meshes[i], MeshVertices) ? zox_get(meshes[i], MeshVertices)->length : -1,
                        zox_getv(meshes[i], RenderDepth));
                }
                return 1;
            }
            return 0;
        }
        zox_tst_sea_streamed = zox_current_time;
    }
    if (zox_current_time - zox_tst_sea_streamed < 4) {
        return 0;
    }
    // a look at it: tst/sea.ppm, and another half a second later (sea_later.ppm) to see it move
    static byte shot = 0;
    static double shot_time = 0;
    if (zox_screenshot_request[0]) {
        return 0;
    }
    if (shot == 0) {
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("sea.ppm"));
        shot = 1;
        shot_time = zox_current_time;
        return 0;
    }
    if (shot == 1) {
        if (zox_current_time - shot_time < 0.5) {
            return 0;
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("sea_later.ppm"));
        shot = 2;
        return 0;
    }
    return 1;
}

void zox_tst_sea_level_check(ecs* world) {
    const entity terrain = zox_tst_world_terrain;
    const byte water_id = zox_valid(terrain) ? zox_tst_sea_water_id(world, terrain) : 0;
    zox_tst_check(water_id, "the realm has no water block");
    if (!water_id) {
        return;
    }
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    const int sea = terrain_sea_height * render_distance_y;
    const int3 center = real_position_to_chunk_position(zox_tst_sea_spot, length, zox_getv(terrain, BlockScale));
    for (int cx = -2; cx <= 2; cx++) for (int cz = -2; cz <= 2; cz++) for (int cy = -render_distance_y; cy <= render_distance_y; cy++) {
        const int3 at = { center.x + cx, cy, center.z + cz };
        const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, at);
        if (!zox_valid(chunk) || !zox_has(chunk, NodeDepth) || zox_getv(chunk, NodeDepth) != depth ||
            (zox_has(chunk, GenerateChunk) && zox_getv(chunk, GenerateChunk)) || zox_has(chunk, Edited)) {
            continue;
        }
        const entity tunk = zox_get_link(world, chunk, TunkLink);
        const HeightMap* heights = zox_valid(tunk) ? zox_get(tunk, HeightMap) : NULL;
        if (!heights || heights->length != length * length) {
            continue;
        }
        zox_tst_sea_chunks++;
        const int bottom = at.y * length;
        const VoxelNode* voxels = zox_get(chunk, VoxelNode);
        for (int x = 0; x < length; x++) for (int z = 0; z < length; z++) {
            const int height = heights->value[z * length + x];
            zox_tst_sea_lowest = height < zox_tst_sea_lowest ? height : zox_tst_sea_lowest;
            for (int y = 0; y < length; y++) {
                const int global = bottom + y;
                const byte voxel = getv_VoxelNode(voxels, depth, (byte3) { x, y, z });
                if (global > sea) {
                    zox_tst_sea_above += voxel == water_id;
                } else if (global > height) {
                    // under the sea, over the ground: water (or what grew or was built there, never air)
                    // towns clear their ground's air (todo.md Bugs: towns in the sea), counted apart
                    const int2 b = { at.x * length + x, at.z * length + z };
                    if (!voxel && town_at(world, b)) {
                        zox_tst_sea_town_air++;
                        continue;
                    }
                    if (!voxel && !zox_tst_sea_wrong) {
                        zox_log("[tst] sea_level: air under the sea at block [%ix%ix%i], its ground [%i]", b.x, global, b.y, height);
                    }
                    zox_tst_sea_wrong += !voxel;
                    zox_tst_sea_columns += global == sea && voxel == water_id;
                }
            }
        }
    }
    // the land's heights over a wide area (every 8 blocks over 2048), to pick the sea's level from
    {
        TerrainHeights th;
        int counts[256] = { 0 }, total = 0;
        if (terrain_heights_begin(world, terrain, &th)) {
            for (int x = -1024; x < 1024; x += 8) for (int z = -1024; z < 1024; z += 8) {
                counts[int_clamp(terrain_height_at(&th, (int2) { x, z }), 0, 255)]++;
                total++;
            }
            int below = 0;
            char line[512] = { 0 };
            for (int h = 0; h < 64 && total; h++) {
                below += counts[h];
                if (h % 3 == 2) {
                    snprintf(line + strlen(line), sizeof(line) - strlen(line), " %i:%i%%", h, below * 100 / total);
                }
            }
            zox_log("[tst] [metric] sea_level land at or below a height, of [%i] samples:%s", total, line);
        }
    }
    zox_log("[tst] [metric] sea_level [%i] chunks checked, sea at [%i], lowest ground [%i], [%i] columns under it, [%i] air under the sea, [%i] water over it",
        zox_tst_sea_chunks, sea, zox_tst_sea_lowest, zox_tst_sea_columns, zox_tst_sea_wrong, zox_tst_sea_above);
    // the water moves on screen: the two screenshots' middles (looking down at the sea) differ
    {
        int2 a_size, b_size;
        byte* a = zox_tst_load_ppm(zox_tst_path("sea.ppm"), &a_size);
        byte* b = zox_tst_load_ppm(zox_tst_path("sea_later.ppm"), &b_size);
        int changed = 0, blue = 0, pixels = 0;
        if (a && b && a_size.x == b_size.x && a_size.y == b_size.y) {
            for (int y = a_size.y * 3 / 8; y < a_size.y * 3 / 4; y++) for (int x = 0; x < a_size.x; x++) {
                const byte* p = &a[(y * a_size.x + x) * 3];
                const byte* q = &b[(y * a_size.x + x) * 3];
                const byte is_blue = p[2] > p[0] + 40 && p[2] > p[1] + 20;
                blue += is_blue;
                changed += is_blue && abs(p[0] - q[0]) + abs(p[1] - q[1]) + abs(p[2] - q[2]) > 12;
                pixels++;
            }
        }
        zox_log("[tst] [metric] sea_level on screen: [%i%%] of the view's middle is water, [%i%%] of it changed in half a second",
            pixels ? blue * 100 / pixels : 0, blue ? changed * 100 / blue : 0);
        zox_tst_check(a && b, "the sea's screenshots weren't saved");
        zox_tst_check(blue * 10 > pixels * 3, "under [30%%] of the view looking down at the sea is water ([%i] of [%i] pixels)", blue, pixels);
        zox_tst_check(changed * 10 > blue, "the water on screen barely moved in half a second ([%i] of [%i] water pixels)", changed, blue);
        free(a);
        free(b);
    }
    zox_tst_check(zox_tst_sea_found, "no ground under the sea within 600 blocks: the sea is too low for this land");
    zox_tst_check(zox_tst_sea_chunks > 10, "only [%i] generated chunks were found to check", zox_tst_sea_chunks);
    zox_tst_check(zox_tst_sea_columns > 0, "no water at the sea's level by ground under it");
    zox_log("[tst] [metric] sea_level [%i] air voxels under the sea in towns (towns in the sea, a known bug)", zox_tst_sea_town_air);
    zox_tst_check(!zox_tst_sea_wrong, "[%i] voxels under the sea and over the ground are air, out of towns", zox_tst_sea_wrong);
    zox_tst_check(!zox_tst_sea_above, "[%i] water voxels are over the sea", zox_tst_sea_above);
}

// tst: lakes, lakes above the sea sit in bowls carved into the land, full to a block under their rim
//  - no player: a realm and terrain of its own (world_only.c), one camera over the nearest lake above the sea to the
//      origin (from the terrain's heights: heights/fun lakes), looking down at it, tst/lake.ppm
//  - its chunks against the heights: in the bowl, water from the ground up to the lake's level, none over it; every
//      water voxel at its surface has water or ground beside it on all 4 sides (it doesn't hang over air)
//  - run: bash bsh/tst.sh lakes
double zox_tst_lake_begin = 0, zox_tst_lake_streamed = 0;
TerrainLake zox_tst_lake;
int zox_tst_lake_level = 0;
byte zox_tst_lake_found = 0;
entity zox_tst_lake_camera = 0;

byte zox_tst_lakes_ready(ecs* world) {
    const entity terrain = zox_tst_world_ready(world);
    TerrainHeights th;
    if (!terrain || !terrain_heights_begin(world, terrain, &th)) {
        return 0;
    }
    const float scale = zox_getv(terrain, BlockScale);
    const int sea = terrain_sea_height * render_distance_y;
    if (!zox_tst_lake_begin) {
        zox_tst_lake_begin = zox_current_time;
        remove(zox_tst_path("lake.ppm"));
        // the nearest cell with a lake above the sea, its middle under water
        for (int r = 0; r <= 12 && !zox_tst_lake_found; r++)
        for (int x = -r; x <= r && !zox_tst_lake_found; x++)
        for (int z = -r; z <= r && !zox_tst_lake_found; z++) {
            if (abs(x) != r && abs(z) != r) {
                continue;
            }
            TerrainLake lake;
            if (!terrain_lake_in_cell(&th, (int2) { x * zox_lake_cell, z * zox_lake_cell }, &lake)) {
                continue;
            }
            const int level = terrain_lake_level(&th, &lake);
            if (level <= sea + 1 || terrain_height_at(&th, lake.centre) >= level) {
                continue;
            }

            zox_tst_lake = lake;
            zox_tst_lake_level = level;
            zox_tst_lake_found = 1;
        }
        zox_log("[tst] lakes: a lake [%s] at block [%ix%i], radius [%i] depth [%i] level [%i] (the sea [%i])", zox_tst_lake_found ? "found" : "not found",
            zox_tst_lake.centre.x, zox_tst_lake.centre.y, zox_tst_lake.radius, zox_tst_lake.depth, zox_tst_lake_level, sea);
        if (!zox_tst_lake_found) {
            return 1;
        }
        const float3 spot = { (zox_tst_lake.centre.x + 0.5f) * scale, (zox_tst_lake_level + 1) * scale, (zox_tst_lake.centre.y + 0.5f) * scale };
        const float3 eye = float3_add(spot, (float3) { 0, (zox_tst_lake.radius + 4) * scale, zox_tst_lake.radius * scale });
        zox_tst_lake_camera = spawn_viewport_camera(world, main_app, eye, camera_look_at(eye, spot), 70, viewport_grid_cell(0, 1, 1));
        prefab_add_streamer(world, zox_tst_lake_camera);
        zox_set_link(world, zox_tst_lake_camera, StreamLink, terrain);
        zox_setv(zox_tst_lake_camera, StreamerLevel, 1);
        nohud = 1;
        const entity game_view = zox_valid(main_cameras[0]) ? zox_get_link(world, main_cameras[0], RenderTextureLink) : 0;
        if (zox_valid(game_view)) {
            zox_setv(game_view, RenderDisabled, 1);
        }
    }
    zox_tst_progress((int) (zox_current_time - zox_tst_lake_begin));
    zox_tst_waiting = "the lake's land to stream in and show its water";
    const float3 centre = { zox_tst_lake.centre.x * scale, zox_tst_lake_level * scale, zox_tst_lake.centre.y * scale };
    const int3 chunk_position = real_position_to_chunk_position(centre, octree_size(zox_getv(terrain, NodeDepth)), scale);
    const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, chunk_position);
    if (!zox_tst_lake_streamed) {
        int water = 0, solid = 0, meshes = 0;
        if (zox_valid(chunk)) {
            zox_tst_water_vertices(world, chunk, &water, &solid, &meshes);
        }
        if (!water) {
            return zox_current_time - zox_tst_lake_begin > 60;
        }
        zox_tst_lake_streamed = zox_current_time;
    }
    if (zox_current_time - zox_tst_lake_streamed < 4) {
        return 0;
    }
    static byte shot = 0;
    if (zox_screenshot_request[0]) {
        return 0;
    }
    if (!shot) {
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("lake.ppm"));
        shot = 1;
        return 0;
    }
    return 1;
}

// a voxel at a block position from full detail generated chunks, -1 if not there
static inline int zox_tst_lake_voxel(ecs* world, entity terrain, int3 block) {
    const byte depth = zox_getv(terrain, NodeDepth);
    const byte length = octree_size(depth);
    const int3 chunk_position = block_position_to_positionc(block, byte3_single(length));
    const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, chunk_position);
    if (!zox_valid(chunk) || !zox_has(chunk, NodeDepth) || zox_getv(chunk, NodeDepth) != depth ||
        (zox_has(chunk, GenerateChunk) && zox_getv(chunk, GenerateChunk)) || zox_has(chunk, Edited))
    {
        return -1;
    }
    return getv_VoxelNode(zox_get(chunk, VoxelNode), depth, get_positionl_byte3(block, byte3_single(length)));
}

void zox_tst_lakes_check(ecs* world) {
    zox_tst_check(zox_tst_lake_found, "no lake above the sea within 12 cells of the origin");
    const entity terrain = zox_tst_world_terrain;
    if (!zox_tst_lake_found || !zox_valid(terrain)) {
        return;
    }
    const int water_id = zox_tst_sea_water_id(world, terrain);
    TerrainHeights th;
    terrain_heights_begin(world, terrain, &th);
    const TerrainLake* lake = &zox_tst_lake;
    const int level = zox_tst_lake_level;
    int columns = 0, wrong = 0, above = 0, surface = 0, hanging = 0, unchecked = 0, in_towns = 0;
    for (int x = -lake->radius; x <= lake->radius; x++) for (int z = -lake->radius; z <= lake->radius; z++) {
        const int2 column = { lake->centre.x + x, lake->centre.y + z };
        if (!terrain_lake_dip(lake, column)) {
            continue;
        }
        // towns build over lakes (a known bug, todo.md), their columns counted apart
        if (town_at(world, column)) {
            in_towns++;
            continue;
        }
        const int ground = terrain_height_at(&th, column);
        const int over = zox_tst_lake_voxel(world, terrain, (int3) { column.x, level + 1, column.y });
        if (over < 0) {
            unchecked++;
            continue;
        }
        above += over == water_id;
        if (ground >= level) {
            continue;
        }
        columns++;
        for (int y = ground + 1; y <= level; y++) {
            const int v = zox_tst_lake_voxel(world, terrain, (int3) { column.x, y, column.y });
            if (v >= 0 && v != water_id && !wrong) {
                const entity block = v ? zox_get_parent(world, terrain) : 0;
                const BlockLinks* blocks = zox_valid(block) ? zox_get(block, BlockLinks) : NULL;
                zox_log("[tst] lakes: block [%ix%ix%i] is [%s] (voxel %i) not water, the ground there [%i]", column.x, y, column.y,
                    blocks && v - 1 < blocks->length ? zox_getn(blocks->value[v - 1]) : "air", v, ground);
            }
            wrong += v >= 0 && v != water_id;
        }
        // its surface held in: water or ground on all 4 sides
        surface++;
        const int2 sides[4] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };
        for (byte k = 0; k < 4; k++) {
            const int2 side = { column.x + sides[k].x, column.y + sides[k].y };
            hanging += !town_at(world, side) && !zox_tst_lake_voxel(world, terrain, (int3) { side.x, level, side.y });
        }
    }
    zox_log("[tst] [metric] lakes: [%i] columns under its water, [%i] voxels in them not water, [%i] water over its level, [%i] surface sides over air, [%i] columns not loaded, [%i] in towns",
        columns, wrong, above, hanging, unchecked, in_towns);
    zox_tst_check(columns > 10, "only [%i] columns are under the lake", columns);
    zox_tst_check(!wrong, "[%i] voxels under the lake's level in its bowl aren't water", wrong);
    zox_tst_check(!above, "[%i] water voxels are over the lake's level", above);
    zox_tst_check(!hanging, "[%i] sides of the lake's surface are open to air: it spills", hanging);
    FILE* file = fopen(zox_tst_path("lake.ppm"), "rb");
    zox_tst_check(file, "the lake's screenshot wasn't saved");
    if (file) {
        fclose(file);
    }
}
