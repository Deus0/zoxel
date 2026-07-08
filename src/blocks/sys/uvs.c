
// uses terrain's texture links to generate a tilemap
// todo: support for multiple sizes, would have to place them in? or something
zox_sys2(TilemapUVSystem) {
    byte dbg_log = 0;
    byte faces_per_block = 6;
    byte uvs_per_face = 4;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureDirty);
    zox_sys_in(TextureLinks);
    zox_sys_in(TilemapSize);
    zox_sys_in(TextureSize);
    zox_sys_in(RealmLink);
    zox_sys_out(TilemapUVs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TextureDirty, state);
        zox_sys_i(TextureLinks, textures);
        zox_sys_i(TilemapSize, map_size);
        zox_sys_i(TextureSize, size);
        zox_sys_i(RealmLink, realm);
        zox_sys_o(TilemapUVs, uvs);
        if (state->value != zox_dirty_active) {
            continue;
        }
        if (!textures->length || !map_size->value.x) {
            zox_loge("Invalid Textures for Tilemap");
            continue;
        }
        entity first_texture = textures->value[0];
        if (!zox_valid(first_texture)) {
            zox_loge("Invalid [first_texture] for Tilemap");
            continue;
        }
        if (!zox_valid(realm->value)) {
            zox_loge("Invalid [realm] for Tilemap");
            continue;
        }
        zox_geter(realm->value, BlockLinks, blocks);
        int2 unit_size = zox_getv(first_texture, TextureSize);
        uint uvs_length = blocks->length * 6 * 4;
        resize_TilemapUVs(uvs, uvs_length);
        float unit_sizef = 1.0f / ((float) map_size->value.x);
        int texture_index = 0;
        for (int y = 0; y < map_size->value.y; y++) {
            for (int x = 0; x < map_size->value.x; x++) {
                entity texture = textures->value[texture_index];
                if (!zox_valid(texture)) {
                    zox_loge("Texture[%i] Invalid", texture_index);
                    texture_index++;
                    continue;
                }
                // Get block linked from texture
                entity block = get_block_link(world, texture);
                if (!zox_valid(block) || !zox_has(block, TextureLinks)) {
                    zox_loge("Texture [%s] Block Invalid", zox_get_name(texture));
                    texture_index++;
                    continue;
                }
                float2 tile_uv = (float2) {
                    (x * unit_size.x) / (float) size->value.x,
                    (y * unit_size.y) / (float) size->value.y
                };
                zox_geter(block, TextureLinks, block_textures);
                byte block_index = zox_getv(block, BlockIndex) - 1;
                // Now
                byte face_index = 0;
                for (byte k = 0; k < block_textures->length; k++) {
                    if (block_textures->value[k] == texture) {
                        face_index = k;
                        break;
                    }
                }
                int block_uv_index = block_index * faces_per_block * uvs_per_face;
                int uv_index = block_uv_index + face_index * uvs_per_face;
                if (dbg_log && block_index == 1) {
                    zox_log("   - Block [%i], Face [%i], UV Index [%i]", block_index, face_index, uv_index);
                }
                // NOTE: This is for single texture blocks
                byte repeated = block_textures->length == 1 ? faces_per_block : 1;
                for (byte k = 0; k < repeated; k++) {
                    uvs->value[uv_index + 0] = float2_add(tile_uv, float2_mul1(voxel_face_uvs[face_index][0], unit_sizef));
                    uvs->value[uv_index + 1] = float2_add(tile_uv, float2_mul1(voxel_face_uvs[face_index][1], unit_sizef));
                    uvs->value[uv_index + 2] = float2_add(tile_uv, float2_mul1(voxel_face_uvs[face_index][2], unit_sizef));
                    uvs->value[uv_index + 3] = float2_add(tile_uv, float2_mul1(voxel_face_uvs[face_index][3], unit_sizef));
                    // expand by new face
                    /*uvs->value[uv_index + 3] = (float2) {
                        tile_uv.x,
                        tile_uv.y + unit_sizef
                    };
                    uvs->value[uv_index + 2] = (float2) {
                        tile_uv.x + unit_sizef,
                        tile_uv.y + unit_sizef
                    };
                    uvs->value[uv_index + 1] = (float2) {
                        tile_uv.x + unit_sizef,
                        tile_uv.y
                    };
                    uvs->value[uv_index + 0] = (float2) {
                        tile_uv.x,
                        tile_uv.y
                    };*/
                    uv_index += uvs_per_face;
                }
                texture_index++;
                if (texture_index >= textures->length) {
                    break;
                }
            }
            if (texture_index >= textures->length) {
                break;
            }
        }
        if (dbg_log) {
            zox_log("UVs Length [%i] Blocks [%i] Textures [%i] MapSize [%ix%i]", uvs_length, blocks->length, textures->length, map_size->value.x, map_size->value.y);
        }
    }
} zox_sys_end(TilemapUVSystem);
