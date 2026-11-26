// tst: water_block, the realm's water: see through, walked through, its texture moves
//  - the realm has a water block: a liquid material, an air collider, light passing
//  - its texture is its own (not on the tilemap), on the gpu, its pixels changing as it animates; its mesh links it
//      and its faces' uvs span it
//  - placed in an all air chunk (air_chunk.c: a world of its own, no player): two water blocks side by side in a pool of bricks open at the top (so they stay)
//      water draws a blended mesh (TransparentMesh) with only its faces against air (its tops): none between the
//      waters or against the bricks; the bricks keep their faces against the water
//  - run: bash bsh/tst.sh water_block
byte zox_tst_water_stage = 0;
double zox_tst_water_time = 0;
entity zox_tst_water_chunk = 0;
uint zox_tst_water_pixels = 0;

// a texture's pixels summed, to see it change
static inline uint zox_tst_water_pixels_sum(ecs* world, entity texture) {
    const TextureData* data = zox_get(texture, TextureData);
    uint sum = 0;
    for (int i = 0; i < data->length; i++) {
        const color c = data->value[i];
        sum += c.r * 3 + c.g * 5 + c.b * 7 + i * (c.r + c.g + c.b);
    }
    return sum;
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

// the pool: x y z, and w 1 for water: the waters at 4 and 5 (y 8, z 8), bricks under, beside and at their ends
#define zox_tst_water_pool_count 10
const int4 zox_tst_water_pool[zox_tst_water_pool_count] = {
    { 4, 8, 8, 1 }, { 5, 8, 8, 1 },
    { 4, 7, 8, 0 }, { 5, 7, 8, 0 }, { 3, 8, 8, 0 }, { 6, 8, 8, 0 },
    { 4, 8, 7, 0 }, { 5, 8, 7, 0 }, { 4, 8, 9, 0 }, { 5, 8, 9, 0 } };

// what's in the pool at a position: 2 water, 1 a brick, 0 air
static inline byte zox_tst_water_pool_at(int x, int y, int z) {
    for (byte i = 0; i < zox_tst_water_pool_count; i++) {
        const int4 p = zox_tst_water_pool[i];
        if (p.x == x && p.y == y && p.z == z) {
            return p.w ? 2 : 1;
        }
    }
    return 0;
}

byte zox_tst_water_block_ready(ecs* world) {
    const entity air = zox_tst_streamed_air_chunk(world);
    if (!air) {
        return 0;
    }
    const entity realm = zox_tst_world_realm;
    zox_tst_progress(zox_tst_water_stage * 1000 + (int) (zox_current_time - zox_tst_water_time));
    if (zox_tst_water_stage == 0) {
        zox_tst_water_chunk = air;
        const entity water = zox_get_child_by_id(world, realm, zox_id(BlockWater));
        const entity bricks = zox_get_child_by_id(world, realm, zox_id(BlockBricks));
        zox_tst_check(zox_valid(water) && zox_valid(bricks), "the realm has no water or bricks block");
        if (!zox_valid(water) || !zox_valid(bricks)) {
            return 1;
        }
        // two water blocks in a pool of bricks open at the top, so they neither fall nor spread (water_flow.c)
        VoxelNodeQueue* queue = zox_getm(zox_tst_water_chunk, VoxelNodeQueue);
        for (byte i = 0; i < zox_tst_water_pool_count; i++) {
            const byte is_water = zox_tst_water_pool[i].w;
            a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = zox_getv(is_water ? water : bricks, BlockIndex),
                .position = (byte3) { zox_tst_water_pool[i].x, zox_tst_water_pool[i].y, zox_tst_water_pool[i].z } });
        }
        const entity texture = zox_get_child_by_id(world, water, zox_id(Texture));
        if (zox_valid(texture)) {
            zox_tst_water_pixels = zox_tst_water_pixels_sum(world, texture);
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
    const entity realm = zox_tst_world_realm;
    const entity water = zox_get_child_by_id(world, realm, zox_id(BlockWater));
    zox_tst_check(zox_valid(water), "the realm has no water block");
    if (!zox_valid(water) || !zox_valid(zox_tst_water_chunk)) {
        return;
    }
    zox_tst_check(zox_getv(water, BlockMaterial) == zox_block_material_liquid, "water isn't a liquid material");
    zox_tst_check(zox_getv(water, BlockCollider) == zox_block_air, "water isn't walked through");
    zox_tst_check(zox_getv(water, BlockLightPass), "water stops light");
    // its own texture, off the tilemap, on the gpu, moving
    const entity texture = zox_get_child_by_id(world, water, zox_id(Texture));
    zox_tst_check(zox_valid(texture) && zox_has(texture, TextureFrames), "water has no animated texture");
    if (zox_valid(texture) && zox_has(texture, TextureFrames)) {
        const entity tilemap = zox_get_link(world, zox_tst_water_chunk, Tilemap);
        const byte on_tilemap = zox_is_linked(world, tilemap, TilemapTextureLinks, texture);
        const uint now = zox_tst_water_pixels_sum(world, texture);
        const TextureFrames* frames = zox_get(texture, TextureFrames);
        const int2 size = zox_getv(texture, TextureSize);
        zox_log("[tst] [metric] water_block its texture [%ix%i] gpu [%i] on the tilemap [%i]: [%i] frames, showing [%i], pixels [%u] then [%u]",
            size.x, size.y, zox_getv(texture, TextureGPULink), on_tilemap, frames->length / (size.x * size.y), zox_getv(texture, TextureFrame),
            zox_tst_water_pixels, now);
        zox_tst_check(!on_tilemap, "the water texture is on the tilemap: it should draw alone");
        zox_tst_check(zox_getv(texture, TextureGPULink), "the water texture has no gpu texture");
        zox_tst_check(frames->length / (size.x * size.y) == zox_water_frames, "water has [%i] frames, not [%i]", frames->length / (size.x * size.y), zox_water_frames);
        zox_tst_check(now != zox_tst_water_pixels, "the water texture didn't change in [%.1fs]", zox_current_time - zox_tst_water_time);
        zox_tst_check(size.x == powers_of_two[block_depth], "the water texture is [%i] across, a block's [%i]", size.x, powers_of_two[block_depth]);
    }
    // its faces
    int vertices, solid, water_meshes;
    zox_tst_water_vertices(world, zox_tst_water_chunk, &vertices, &solid, &water_meshes);
    zox_log("[tst] [metric] water_block meshes: water [%i] ([%i] faces, [%i] meshes), bricks [%i] ([%i] faces)", vertices, vertices / 4,
        water_meshes, solid, solid / 4);
    // its faces' uvs span its whole texture, its mesh linked to it
    {
        entity meshes[zox_chunk_meshes_max];
        const uint count = get_chunk_meshes(world, zox_tst_water_chunk, meshes);
        const int2 tiles = int2_one;
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
                    zox_log("[tst] [metric] water_block a face's uvs span [%.3f x %.3f] of its texture, linked to it [%i]", span.x, span.y, zox_get_link(world, meshes[i], MeshTextureLink) == texture);
                    zox_tst_check(zox_get_link(world, meshes[i], MeshTextureLink) == texture, "the water mesh doesn't link the water texture");
                }
                zox_tst_check(fabsf(span.x - 1) < 0.01f && fabsf(span.y - 1) < 0.01f, "a water face's uvs span [%.3f x %.3f] of its texture, not all of it",
                    span.x, span.y);
            }
        }
    }
    zox_tst_check(water_meshes == 1, "[%i] see through meshes shown, not one", water_meshes);
    // the faces expected: water's only against air (its tops), a brick's against air and against water
    int water_faces = 0, brick_faces = 0;
    for (byte i = 0; i < zox_tst_water_pool_count; i++) {
        const int4 p = zox_tst_water_pool[i];
        for (byte d = 0; d < 6; d++) {
            const sbyte3 o = neighbor_offsets[d];
            const byte beside = zox_tst_water_pool_at(p.x + o.x, p.y + o.y, p.z + o.z);
            if (p.w) {
                water_faces += beside == 0;
            } else {
                brick_faces += beside != 1;
            }
        }
    }
    zox_tst_check(vertices == water_faces * 4, "the water has [%i] faces, not its [%i] against air (none between its blocks or against the bricks)",
        vertices / 4, water_faces);
    zox_tst_check(solid == brick_faces * 4, "the bricks have [%i] faces, not their [%i] against air and water (their faces against water show)",
        solid / 4, brick_faces);
}

// the realm's water block's index, 0 if none
static inline byte zox_tst_sea_water_id(ecs* world, entity terrain) {
    const entity water = zox_get_child_by_id(world, zox_get_parent(world, terrain), zox_id(BlockWater));
    return zox_valid(water) ? zox_getv(water, BlockIndex) : 0;
}
