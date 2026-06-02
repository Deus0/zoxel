// uses terrain's texture links to generate a tilemap
// todo: support for multiple sizes, would have to place them in? or something
extern entity get_block_link(ecs*, entity);

zox_sys2(TilemapGenerationSystem) {
    byte uvs_per_tile = 4;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateTexture);
    zox_sys_in(TilemapSize);
    zox_sys_in(TextureLinks);
    zox_sys_out(TextureSize);
    zox_sys_out(TextureData);
    zox_sys_out(TextureDirty);
    // zox_sys_out(TilemapUVs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateTexture, state);
        zox_sys_i(TilemapSize, tmsize);
        zox_sys_i(TextureLinks, textures);
        zox_sys_o(TextureSize, size);
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureDirty, dirty);
        // zox_sys_o(TilemapUVs, uvs);
        if (state->value != zox_dirty_active || dirty->value) {
            continue;
        }
        if (!textures->length || !tmsize->value.x) {
            zox_log_error("Invalid Textures for Tilemap");
            continue;
        }
        byte still_generating = 0;
        for (int j = 0; j < textures->length; j++) {
            entity texture = textures->value[j];
            if (!zox_valid(texture)) {
                zox_loge("Tilemap Texture Invalid [%i]", j);
                continue;
            }
            // NOTE: THese are the texture_file entities
            if (!zox_has(texture, GenerateTexture)) {
                // entity block = get_block_link(world, texture);
                // zox_loge("Tilemap Texture (of block [%s]) has no GenerateTexture [%i]", zox_get_name(block), j);
                continue;
            }
            if (zox_gett_value(texture, GenerateTexture)) {
                zox_loge("Tilemap Texture still generating [%i]", j);
                still_generating = 1;
                break;
            }
        }
        if (still_generating) {
            zox_log("Tilemap still generating...");
            zox_set(e, GenerateTexture, { zox_dirty_trigger });
            continue;
        }
        // generate size based on TilemapSize
        entity first_texture = textures->value[0];
        if (!zox_valid(first_texture)) {
            zox_log_error("Invalid First Texture for Tilemap");
            continue;
        }
        int2 unit_size = zox_getv(first_texture, TextureSize);
        // for example, 1 / 8 if size is 8
        // zox_log("Tilemap Size %i - u%i", tmsize->value.x, unit_size.x);
        size->value.x = tmsize->value.x * unit_size.x;
        size->value.y = tmsize->value.y * unit_size.y;
        resize_TextureData(data, size->value.x * size->value.y);
        int2 texture_position = int2_zero;
        int texture_index = 0;
        for (int j = 0; j < data->length; j++) {
            data->value[j] = color_white;
        }
        for (texture_position.y = 0; texture_position.y < tmsize->value.y && texture_index < textures->length; texture_position.y++) {
            for (texture_position.x = 0; texture_position.x < tmsize->value.x && texture_index < textures->length; texture_position.x++) {
                entity texture = textures->value[texture_index];
                if (!zox_valid(texture) || !zox_has(texture, TextureData)) {
                    zox_log_error("invalid texture [%s] index [%i]", zox_get_name(texture), texture_index)
                    texture_index++;
                    continue;
                }
                zox_geter(texture, TextureData, texture_data);
                if (!texture_data->value) {
                    zox_log_error("invalid texture data [%s] index [%i]", zox_get_name(texture), texture_index);
                    texture_index++;
                    continue;
                }
                zox_geter_value(texture, TextureSize, int2, texture_size);
                int2 tilemap_position = (int2) {
                    texture_position.x * unit_size.x,
                    texture_position.y * unit_size.y
                };
                // tod: refactor - make this into a function
                int2 pixel_position = int2_zero;
                for (pixel_position.x = 0; pixel_position.x < texture_size.x; pixel_position.x++) {
                    for (pixel_position.y = 0; pixel_position.y < texture_size.y; pixel_position.y++) {
                        int2 tilemap_pixel_position = int2_add(pixel_position, tilemap_position);
                        int tilemap_index = int2_array_index(tilemap_pixel_position, size->value);
                        if (tilemap_index >= data->length) {
                            zox_log_error("tilemap_index [%i] >= data->length [%i]", tilemap_index, data->length);
                            continue;
                        }
                        int texture_index = int2_array_index(pixel_position, texture_size);
                        if (texture_index >= texture_data->length) {
                            continue;
                        }
                        data->value[tilemap_index] = texture_data->value[texture_index];
                    }
                }
                texture_index++;
            }
        }
        // Set dirty here
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(TilemapGenerationSystem);

        /*if (!textures->length) {
            zox_loge("Tilemap has no Textures...");
            continue;
        }
        // TODO: Refactor uvs into another system, needs block data
        texture_index = 0;
        int uv_texture_index = 0;
        // resize_TilemapUVs(uvs, 0); // textures->length * uvs_per_tile);
        resize_TilemapUVs(uvs, blocks->length * uvs_per_tile);
        float unit_sizef = 1.0f / ((float) tmsize->value.x);
        for (texture_position.y = 0; texture_position.y < tmsize->value.y; texture_position.y++) {
            for (texture_position.x = 0; texture_position.x < tmsize->value.x; texture_position.x++) {
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
                int block_textures_count = zox_gett(block, TextureLinks)->length;
                // if only one texture
                int2 tilemap_position = (int2) {
                    texture_position.x * unit_size.x,
                    texture_position.y * unit_size.y
                };
                float2 tile_uv = (float2) {
                    tilemap_position.x / (float) size->value.x,
                    tilemap_position.y / (float) size->value.y
                };
                // 4 uvs per face
                // zox_log("block_textures_count [%i]", block_textures_count);
                byte repeated = block_textures_count == 1 ? 6 : 1;  // repeat if single texture
                for (byte k = 0; k < repeated; k++) {
                    // expand by new face
                    resize_TilemapUVs(uvs, uvs->length + uvs_per_tile);
                    uvs->value[uv_texture_index * 4 + 3] = (float2) { tile_uv.x, tile_uv.y + unit_sizef };
                    uvs->value[uv_texture_index * 4 + 2] = (float2) { tile_uv.x + unit_sizef, tile_uv.y + unit_sizef };
                    uvs->value[uv_texture_index * 4 + 1] = (float2) { tile_uv.x + unit_sizef, tile_uv.y };
                    uvs->value[uv_texture_index * 4 + 0] = (float2) { tile_uv.x, tile_uv.y };
                    uv_texture_index++;
                }
                texture_index++;
                if (texture_index >= textures->length) {
                    break;
                }
            }
            if (texture_index >= textures->length) {
                break;
            }
        }*/
