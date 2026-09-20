
// uses terrain's texture links to generate a tilemap
// todo: support for multiple sizes, would have to place them in? or something
zox_sys2(TilemapUVSystem) {
    byte dbg_log = 1;
    byte faces_per_block = 6;
    byte uvs_per_face = 4;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TilemapSize);
    zox_sys_in(TextureSize);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TilemapUVs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TilemapSize, map_size);
        zox_sys_i(TextureSize, size);
        zox_sys_o(GenerateTexture, generate);
        zox_sys_o(TilemapUVs, uvs);
        if (generate->value != zox_generate_texture_uvs) {
            continue;
        }
        if (!map_size->value.x) {
            zox_loge("Invalid Textures for Tilemap");
            continue;
        }
        uint texture_index = 0;
        byte generating = 0;
        entity first_texture = 0;
        while (1) {
            entity texture = zox_get_link_index(
                world,
                e,
                TextureLink,
                texture_index);
            texture_index++;
            if (!texture) {
                break;
            }
            if (!first_texture) {
                first_texture = texture;
            }
            if (zox_has(texture, GenerateTexture) &&
                zox_getv(texture, GenerateTexture)
            ) {
                // zox_logw("Block [%s:%i]'s Texture [%s] still generating...", zox_get_name(block), j, zox_get_name(texture));
                generating = 1;
                break;
            }
        }
        if (generating) {
            continue;
        }
        if (!zox_valid(first_texture)) {
            zox_loge("Invalid [first_texture] for Tilemap");
            continue;
        }
        entity realm = zox_get_link(world, e, RealmLink);
        if (!zox_valid(realm)) {
            zox_loge("Invalid [realm] for Tilemap");
            continue;
        }
        zox_geter(realm, BlockLinks, blocks);
        int2 unit_size = zox_getv(first_texture, TextureSize);
        // TODO: Recalculate this based on blocks of Baked Type
        uint uvs_length = blocks->length * 6 * 4;
        resize_TilemapUVs(uvs, uvs_length);
        float unit_sizef = 1.0f / ((float) map_size->value.x);
        texture_index = 0;
        while (1) {
            entity texture = zox_get_link_index(
                world,
                e,
                TextureLink,
                texture_index);
            texture_index++;
            if (!texture) {
                break;
            }
            // Get block linked from texture
            entity block = get_block_link(world, texture);
            if (!zox_valid(block)) {
                zox_loge("Texture [%s] Block Invalid",
                    zox_get_name(texture));
                continue;
            }
            uint tilemap_index = zox_getv(texture, TilemapIndex);
            byte block_index = zox_getv(block, BlockIndex) - 1;
            // Now
            byte face_index =
                zox_has(texture, ChildIndex) ?
                    zox_getv(texture, ChildIndex) :
                    0;
            byte single_texture = zox_has(block, SingleTextureBlock);
            // NOTE: This is for single texture blocks
            byte faces =
                single_texture ?
                    faces_per_block :
                    1;
            int block_uv_index = block_index * faces_per_block * uvs_per_face;
            int uv_index =
                block_uv_index +
                (single_texture ? 0 : face_index * uvs_per_face);
            int x =
            tilemap_index % map_size->value.x;
            int y =
            tilemap_index / map_size->value.x;
            float2 tile_uv = (float2) {
                (x * unit_size.x) / (float) size->value.x,
                (y * unit_size.y) / (float) size->value.y
            };
                // face_index * uvs_per_face;
            // block_textures->length == 1 ? faces_per_block : 1;
            for (byte k = 0; k < faces; k++) {
                byte uv_face =
                    single_texture
                        ? k
                        : face_index;
                uvs->value[uv_index + 0] =
                    float2_add(
                        tile_uv,
                        float2_mul1(
                            voxel_face_uvs[uv_face][0], unit_sizef));
                uvs->value[uv_index + 1] =
                    float2_add(
                        tile_uv,
                        float2_mul1(
                            voxel_face_uvs[uv_face][1],
                            unit_sizef));
                uvs->value[uv_index + 2] =
                    float2_add(
                        tile_uv,
                        float2_mul1(
                            voxel_face_uvs[uv_face][2],
                            unit_sizef));
                uvs->value[uv_index + 3] =
                    float2_add(
                        tile_uv,
                        float2_mul1(
                            voxel_face_uvs[uv_face][3],
                            unit_sizef));
                uv_index += uvs_per_face;
            }
            if (dbg_log >= 2) { // && block_index == 1) {
                zox_log("   - Block [%i], Face [%i], UV Index [%i]",
                    block_index,
                    face_index,
                    uv_index);
            }
        }
        generate->value = 0; // zox_generate_texture_run;
        if (dbg_log) {
            zox_log("Tilemap UVs Generated: Unit Size [%ix%i] Blocks [%i] Textures [%i] MapSize [%ix%i]",
                unit_size.x,
                unit_size.y,
                blocks->length,
                texture_index,
                map_size->value.x,
                map_size->value.y);
        }
    }
} zox_sys_end(TilemapUVSystem);


/*uvs->value[uv_index + 0] = float2_add(tile_uv, float2_mul1(voxel_face_uvs[face_index][0], unit_sizef));
 u vs->value[uv_index + 1] = float2_add(tile_uv, float2_mul1(voxel_face_uvs[face_index][1], unit_sizef));  *
 uvs->value[uv_index + 2] = float2_add(tile_uv, float2_mul1(voxel_face_uvs[face_index][2], unit_sizef));
 uvs->value[uv_index + 3] = float2_add(tile_uv, float2_mul1(voxel_face_uvs[face_index][3], unit_sizef));*/

/* iter texture_it2 = zox_links(e, TextureLink);
 w hile (zox_*query_next(texture_it2)) {
 for (int j = 0; j < texture_it2.count; j++) {
     entity texture = zox_pair_target(texture_it2, j);*/
/*
 f or (int y *= 0; y < map_size->value.y; y++) {
 for (int x = 0; x < map_size->value.x; x++) {
     entity texture = textures->value[texture_index];
     if (!zox_valid(texture)) {
         zox_loge("Texture[%i] Invalid", texture_index);
         texture_index++;
         continue;
         }*/
