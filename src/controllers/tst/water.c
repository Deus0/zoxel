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
    }
    // its faces
    int vertices, solid, water_meshes;
    zox_tst_water_vertices(world, zox_tst_water_chunk, &vertices, &solid, &water_meshes);
    zox_log("[tst] [metric] water_block meshes: water [%i] ([%i] faces, [%i] meshes), bricks [%i] ([%i] faces)", vertices, vertices / 4,
        water_meshes, solid, solid / 4);
    zox_tst_check(water_meshes == 1, "[%i] see through meshes shown, not one", water_meshes);
    zox_tst_check(vertices == 9 * 4, "the water has [%i] faces, not its 9 outside ones (none between its blocks or against the bricks)", vertices / 4);
    zox_tst_check(solid == 6 * 4, "the bricks have [%i] faces, not all 6 (its face against the water shows)", solid / 4);
}

// tst: sea_level, ground below the sea is under water up to it (heights/sys/landfill.c terrain_sea_height)
//  - flies to the nearest ground under the sea (from the terrain's heights) and waits for it to stream in
//  - every generated chunk at full detail near there, against its tunk's heights: a column whose ground is
//      below the sea holds water from just over its ground up to the sea, and nothing over the sea is water
//  - logs how much of the land is at or below each height (over 2048 blocks), to set the sea's level by
//  - run: bash bsh/tst.sh sea_level
int zox_tst_sea_lowest = 255, zox_tst_sea_columns = 0, zox_tst_sea_wrong = 0, zox_tst_sea_above = 0, zox_tst_sea_chunks = 0;

double zox_tst_sea_begin = 0, zox_tst_sea_streamed = 0;
float3 zox_tst_sea_spot;
byte zox_tst_sea_found = 0;

byte zox_tst_sea_level_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const float scale = zox_getv(terrain, BlockScale);
    if (!zox_tst_sea_begin) {
        zox_tst_sea_begin = zox_current_time;
        // the nearest ground under the sea, from the terrain's heights, out to 600 blocks
        TerrainHeights th;
        const int sea = terrain_sea_height * render_distance_y;
        const float3 p = zox_getv(character, Position3D);
        const int2 start = { (int) floorf(p.x / scale), (int) floorf(p.z / scale) };
        if (sea && terrain_heights_begin(world, terrain, &th)) {
            for (int r = 0; r <= 600 && !zox_tst_sea_found; r += 8)
            for (int x = -r; x <= r && !zox_tst_sea_found; x += 8)
            for (int z = -r; z <= r && !zox_tst_sea_found; z += 8) {
                if ((abs(x) != r && abs(z) != r) || terrain_height_at(&th, (int2) { start.x + x, start.y + z }) >= sea - 1) {
                    continue;
                }
                zox_tst_sea_found = 1;
                zox_tst_sea_spot = (float3) { (start.x + x + 0.5f) * scale, (sea + 6) * scale, (start.y + z + 0.5f) * scale };
            }
        }
        zox_log("[tst] sea_level: ground under the sea [%s] at block [%.0fx%.0f], from [%ix%i]", zox_tst_sea_found ? "found" : "not found",
            zox_tst_sea_spot.x / scale, zox_tst_sea_spot.z / scale, start.x, start.y);
    }
    zox_tst_progress((int) (zox_current_time - zox_tst_sea_begin));
    if (!zox_tst_sea_found) {
        return 1;
    }
    // held over it while its land streams in, then a while for its chunks to generate
    zox_tst_waiting = "the land under the sea to stream in";
    zox_setv(character, Position3D, zox_tst_sea_spot);
    zox_setv(character, Velocity3D, float3_zero);
    const int3 chunk = real_position_to_chunk_position(zox_tst_sea_spot, octree_size(zox_getv(terrain, NodeDepth)), scale);
    const entity tunk = int2_hashmap_get(zox_get(terrain, TunkLinks)->value, (int2) { chunk.x, chunk.z });
    if (!zox_valid(tunk)) {
        return zox_current_time - zox_tst_sea_begin > 40;
    }
    if (!zox_tst_sea_streamed) {
        zox_tst_sea_streamed = zox_current_time;
    }
    if (zox_current_time - zox_tst_sea_streamed < 6) {
        return 0;
    }
    // a look at it: tst/sea.ppm
    static byte shot = 0;
    if (!shot) {
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("sea.ppm"));
        shot = 1;
        return 0;
    }
    return !zox_screenshot_request[0];
}

void zox_tst_sea_level_check(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    const entity water = zox_get_child_by_id(world, realm, zox_id(BlockWater));
    zox_tst_check(zox_valid(water), "the realm has no water block");
    if (!zox_valid(water)) {
        return;
    }
    const byte water_id = zox_getv(water, BlockIndex);
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    const int sea = terrain_sea_height * render_distance_y;
    const int3 center = real_position_to_chunk_position(zox_getv(character, Position3D), length, zox_getv(terrain, BlockScale));
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
    zox_tst_check(zox_tst_sea_found, "no ground under the sea within 600 blocks: the sea is too low for this land");
    zox_tst_check(zox_tst_sea_chunks > 10, "only [%i] generated chunks were found to check", zox_tst_sea_chunks);
    zox_tst_check(zox_tst_sea_columns > 0, "no water at the sea's level by ground under it");
    zox_tst_check(!zox_tst_sea_wrong, "[%i] voxels under the sea and over the ground are air", zox_tst_sea_wrong);
    zox_tst_check(!zox_tst_sea_above, "[%i] water voxels are over the sea", zox_tst_sea_above);
}
