// uses terrain's texture links to generate a tilemap
// todo: support for multiple sizes, would have to place them in? or something
zox_sys2(TilemapGenerationSystem) {
    byte uvs_per_tile = 4;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TilemapSize);
    zox_sys_in(TextureLinks);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureSize);
    zox_sys_out(TextureData);
    zox_sys_out(TextureDirty);
    zox_sys_out(TilemapUVs);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TilemapSize, tmsize);
        zox_sys_i(TextureLinks, textures);
        zox_sys_o(GenerateTexture, state);
        zox_sys_o(TextureSize, tsize);
        zox_sys_o(TextureData, data);
        zox_sys_o(TextureDirty, dirty);
        zox_sys_o(TilemapUVs, uvs);
        if (state->value != zox_dirty_active || dirty->value) {
            continue;
        }
        if (!textures->length || !tmsize->value.x) {
            zox_log_error("Invalid Textures for Tilemap");
            continue;
        }
        // generate tsize based on TilemapSize
        entity first_texture = textures->value[0];
        if (!zox_valid(first_texture)) {
            zox_log_error("Invalid First Texture for Tilemap");
            continue;
        }
        zox_geter_value(first_texture, TextureSize, int2, unit_size);
        float unit_sizef = 1.0f / ((float) tmsize->value.x);
        // for example, 1 / 8 if size is 8
        // zox_log("Tilemap Size %i - u%i", tmsize->value.x, unit_size.x);
        tsize->value.x = tmsize->value.x * unit_size.x;
        tsize->value.y = tmsize->value.y * unit_size.y;
        // resize_memory_component(TextureData, data, color, tsize->value.x * tsize->value.y);
        resize_TextureData(data, tsize->value.x * tsize->value.y);
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
                zox_geter(texture, TextureData, block_texture);
                if (!block_texture->value) {
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
                        int tilemap_index = int2_array_index(tilemap_pixel_position, tsize->value);
                        if (tilemap_index >= data->length) {
                            zox_log_error("tilemap_index [%i] >= data->length [%i]", tilemap_index, data->length);
                            continue;
                        }
                        int texture_index = int2_array_index(pixel_position, texture_size);
                        if (texture_index >= block_texture->length) {
                            continue;
                        }
                        data->value[tilemap_index] = block_texture->value[texture_index];
                    }
                }
                texture_index++;
            }
        }
        texture_index = 0;
        resize_TilemapUVs(uvs, textures->length * uvs_per_tile);
        for (texture_position.y = 0; texture_position.y < tmsize->value.y; texture_position.y++) {
            for (texture_position.x = 0; texture_position.x < tmsize->value.x; texture_position.x++) {
                if (texture_index >= textures->length) {
                    break;
                    break;
                }
                int2 tilemap_position = (int2) {
                    texture_position.x * unit_size.x,
                    texture_position.y * unit_size.y
                };
                float2 tile_uv = (float2) {
                    tilemap_position.x / (float) tsize->value.x,
                    tilemap_position.y / (float) tsize->value.y
                };
                // 4 uvs per face
                uvs->value[texture_index * 4 + 3] = (float2) { tile_uv.x, tile_uv.y + unit_sizef };
                uvs->value[texture_index * 4 + 2] = (float2) { tile_uv.x + unit_sizef, tile_uv.y + unit_sizef };
                uvs->value[texture_index * 4 + 1] = (float2) { tile_uv.x + unit_sizef, tile_uv.y };
                uvs->value[texture_index * 4 + 0] = (float2) { tile_uv.x, tile_uv.y };
                texture_index++;
                if (texture_index >= textures->length) {
                    break;
                    break;
                }
            }
        }
        dirty->value = zox_dirty_trigger;
    }
} zox_sys_end(TilemapGenerationSystem);
