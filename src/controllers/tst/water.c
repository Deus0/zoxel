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
