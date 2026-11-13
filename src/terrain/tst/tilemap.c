// tst: tilemap
//  - waits for a new realm's tilemap + a built textured terrain chunk mesh
//  - every baked block texture is linked to the tilemap once
//  - tilemap pixels at tile i == linked texture i (generation order)
//  - each block face's tilemap uvs point at the tile of its texture (uv order)
//  - every face of the chunk mesh samples a used tile
entity zox_tst_tilemap_mesh = 0;   // found by tst_tilemap_probe_system

// tile index of a uv inside a map of map_size x map_size tiles
static inline int zox_tst_tilemap_tile(float2 uv, int map_size) {
    const int x = (int) (uv.x * map_size);
    const int y = (int) (uv.y * map_size);
    if (x < 0 || y < 0 || x >= map_size || y >= map_size) {
        return -1;
    }
    return x + y * map_size;
}

// center of a face, so we never sample a shared tile edge
static inline float2 zox_tst_quad_center(const float2* uvs) {
    return (float2) {
        (uvs[0].x + uvs[1].x + uvs[2].x + uvs[3].x) / 4.0f,
        (uvs[0].y + uvs[1].y + uvs[2].y + uvs[3].y) / 4.0f
    };
}

byte zox_tst_tilemap_is_ready(ecs* world, entity tilemap) {
    if (!zox_valid(tilemap) || zox_getv(tilemap, GenerateTexture)) {
        return 0;
    }
    const TilemapUVs* uvs = zox_get(tilemap, TilemapUVs);
    return uvs->length && zox_getv(tilemap, TextureGPULink);
}

byte zox_tst_tilemap_ready(ecs* world) {
    entity mesh = zox_tst_tilemap_mesh;
    return zox_valid(mesh) &&
        zox_tst_tilemap_is_ready(world, zox_get_link(world, mesh, MaterialLink));
}

// Checks a tilemap against its realm's blocks
void zox_tst_tilemap_check_tilemap(ecs* world, entity tilemap) {
    entity realm = zox_get_link(world, tilemap, RealmLink);
    zox_tst_check(zox_valid(realm), "tilemap [%s] has no realm", zox_getn(tilemap));
    if (!zox_valid(realm)) {
        return;
    }
    // expected textures, gathered the same way realm_tilemap_system does
    const BlockLinks* blocks = zox_get(realm, BlockLinks);
    int expected = 0;
    for (int j = 0; j < blocks->length; j++) {
        entity block = blocks->value[j];
        if (!zox_valid(block) || !zox_has(block, BlockBaked)) {
            continue;
        }
        iter it2 = zox_children(world, block);
        while (zox_children_next(it2)) {
            for (int k = 0; k < it2.count; k++) {
                entity texture = it2.entities[k];
                if (!zox_has(texture, Texture)) {
                    continue;
                }
                expected++;
                zox_tst_check(
                    zox_is_linked(world, tilemap, TilemapTextureLinks, texture),
                    "block [%s] texture [%s] not linked to tilemap",
                    zox_getn(block), zox_getn(texture));
            }
        }
    }
    const int count = zox_get_links_count(world, tilemap, TilemapTextureLinks);
    zox_tst_check(count == expected,
        "tilemap links [%i] textures, expected [%i]", count, expected);
    zox_tst_check(count, "tilemap has no textures");
    const int map_size = zox_getv(tilemap, TilemapSize).x;
    const int2 size = zox_getv(tilemap, TextureSize);
    const TextureData* data = zox_get(tilemap, TextureData);
    const TilemapUVs* tilemap_uvs = zox_get(tilemap, TilemapUVs);
    zox_log("[tst] tilemap [%s]: [%i] textures, [%ix%i] tiles, [%ix%i] px",
        zox_getn(tilemap), count, map_size, map_size, size.x, size.y);
    zox_tst_check(map_size * map_size >= count,
        "TilemapSize [%i] too small for [%i] textures", map_size, count);
    zox_tst_check(data->value && data->length == size.x * size.y,
        "TextureData length [%i] != TextureSize [%ix%i]", data->length, size.x, size.y);
    zox_tst_check(tilemap_uvs->length == blocks->length * 6 * 4,
        "TilemapUVs length [%i], expected [%i]", tilemap_uvs->length, blocks->length * 6 * 4);
    if (!count || map_size * map_size < count || !data->value || data->length != size.x * size.y) {
        return;
    }
    for (int i = 0; i < count; i++) {
        entity texture = zox_get_link_index(world, tilemap, TilemapTextureLinks, i);
        zox_tst_check(zox_valid(texture) && zox_has(texture, TextureData),
            "link [%i] is not a texture", i);
        if (!zox_valid(texture) || !zox_has(texture, TextureData)) {
            continue;
        }
        for (int j = 0; j < i; j++) {
            zox_tst_check(zox_get_link_index(world, tilemap, TilemapTextureLinks, j) != texture,
                "texture [%s] linked twice [%i] [%i]", zox_getn(texture), j, i);
        }
        // pixels
        const TextureData* texture_data = zox_get(texture, TextureData);
        const int2 texture_size = zox_getv(texture, TextureSize);
        const int2 tile_position = (int2) {
            (i % map_size) * texture_size.x,
            (i / map_size) * texture_size.y
        };
        int bad_pixels = 0;
        for (int x = 0; x < texture_size.x; x++) {
            for (int y = 0; y < texture_size.y; y++) {
                const int pixel_index = x + y * texture_size.x;
                const int tile_index = (tile_position.x + x) + (tile_position.y + y) * size.x;
                if (pixel_index >= texture_data->length ||
                    tile_index >= data->length ||
                    memcmp(&texture_data->value[pixel_index], &data->value[tile_index], sizeof(color)))
                {
                    bad_pixels++;
                }
            }
        }
        zox_tst_check(!bad_pixels, "tile [%i] texture [%s] has [%i] wrong pixels",
            i, zox_getn(texture), bad_pixels);
        // uvs
        entity block = get_block_link(world, texture);
        zox_tst_check(zox_valid(block), "texture [%s] has no BlockLink", zox_getn(texture));
        if (!zox_valid(block)) {
            continue;
        }
        const int block_index = zox_getv(block, BlockIndex) - 1;
        const byte single = zox_has(block, SingleTextureBlock);
        const byte face = zox_has(texture, ChildIndex) ? zox_getv(texture, ChildIndex) : 0;
        for (byte f = 0; f < 6; f++) {
            if (!single && f != face) {
                continue;
            }
            const int uv_index = block_index * 24 + f * 4;
            const byte in_range = uv_index >= 0 && uv_index + 4 <= tilemap_uvs->length;
            zox_tst_check(in_range, "block [%s] uv index [%i] out of range", zox_getn(block), uv_index);
            if (!in_range) {
                break;
            }
            const int tile = zox_tst_tilemap_tile(
                zox_tst_quad_center(&tilemap_uvs->value[uv_index]),
                map_size);
            zox_tst_check(tile == i,
                "block [%s] face [%i] uvs point to tile [%i], its texture [%s] is at [%i]",
                zox_getn(block), f, tile, zox_getn(texture), i);
        }
    }
}

// Checks a built terrain chunk mesh, every face samples a used tile
void zox_tst_tilemap_check_mesh(ecs* world, entity mesh, entity tilemap) {
    const MeshUVs* uvs = zox_get(mesh, MeshUVs);
    const MeshVertices* vertices = zox_get(mesh, MeshVertices);
    zox_tst_check(uvs->length == vertices->length,
        "mesh [%s] has [%i] uvs for [%i] vertices", zox_getn(mesh), uvs->length, vertices->length);
    const int map_size = zox_getv(tilemap, TilemapSize).x;
    const int count = zox_get_links_count(world, tilemap, TilemapTextureLinks);
    zox_tst_check(uvs->length && !(uvs->length % 4),
        "mesh [%s] uvs [%i] are not quads", zox_getn(mesh), uvs->length);
    if (!uvs->length || uvs->length % 4) {
        return;
    }
    int bad_faces = 0;
    int first_bad_tile = 0;
    for (int i = 0; i < uvs->length; i += 4) {
        const int tile = zox_tst_tilemap_tile(zox_tst_quad_center(&uvs->value[i]), map_size);
        if (tile < 0 || tile >= count) {
            if (!bad_faces) {
                first_bad_tile = tile;
            }
            bad_faces++;
        }
    }
    zox_log("[tst] mesh [%s]: [%i] faces", zox_getn(mesh), uvs->length / 4);
    zox_tst_check(!bad_faces,
        "mesh [%s] has [%i/%i] faces off the used tiles, i.e. tile [%i] (used [%i])",
        zox_getn(mesh), bad_faces, uvs->length / 4, first_bad_tile, count);
}

void zox_tst_tilemap_check(ecs* world) {
    entity mesh = zox_tst_tilemap_mesh;
    entity tilemap = zox_get_link(world, mesh, MaterialLink);
    zox_tst_tilemap_check_tilemap(world, tilemap);
    zox_tst_tilemap_check_mesh(world, mesh, tilemap);
}

// Remembers a built textured terrain chunk mesh, only while the test runs
void tst_tilemap_probe_system(iter* it) {
    zox_sys_on_begin();
    if (!zox_tst_active || zox_tst_active->check != zox_tst_tilemap_check) {
        return;
    }
    zox_sys_world();
    if (zox_valid(zox_tst_tilemap_mesh)) {
        return;
    }
    zox_sys_begin();
    zox_sys_in(MeshUVs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(MeshUVs, uvs);
        if (uvs->length) {
            zox_tst_tilemap_mesh = e;
            return;
        }
    }
    zox_sys_on_end();
} zoxd_system(tst_tilemap_probe_system);

void zox_tsts_tilemap(ecs* world) {
    zox_tst_add(
        "tilemap",
        zox_tst_tilemap_ready,
        zox_tst_tilemap_check,
        90);
    // main thread, in zoxp_render with the other main thread systems: no merges of its own (tst pipeline_syncs)
    zox_system_1(
        tst_tilemap_probe_system,
        zoxp_render,
        [in] rendering.MeshUVs,
        [none] rendering.ChunkMesh,
        [none] rendering3.TexturedMesh3
    );
}
