// uses terrain's texture links to generate a tilemap
// todo: support for multiple sizes, would have to place them in? or something
extern entity get_block_link(ecs*, entity);

// NOTE: Relies on texture size
zox_sys2(TilemapGenerationSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TilemapSize);
    // zox_sys_in(TextureLinks);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureSize);
    zox_sys_out(TextureData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TilemapSize, map_size);
        // zox_sys_i(TextureLinks, textures);
        zox_sys_o(GenerateTexture, generate);
        zox_sys_o(TextureSize, size);
        zox_sys_o(TextureData, data);
        if (generate->value != zox_generate_texture_run) {
            continue;
        }
        if (!map_size->value.x) {
            zox_loge("Invalid Textures for Tilemap");
            continue;
        }
        byte still_generating = 0;
        uint texture_index = 0;
        entity first_texture = 0;
        while (1) {
            entity texture = zox_get_link_index(world, e, TextureLink, texture_index);
            if (!texture) {
                break;
            }
            texture_index++;
        /*iter it2 = zox_links(e, TextureLink);
        while (zox_query_next(it2)) {
            for (int j = 0; j < it2.count; j++) {
                entity texture = zox_pair_target(it2, j);*/
                if (!first_texture) {
                    first_texture = texture;
                }
            /*for (int j = 0; j < textures->length; j++) {
                entity texture = textures->value[j];
                if (!zox_valid(texture)) {
                    zox_loge("Tilemap Texture Invalid [%i]", j);
                    continue;
                }*/
                // NOTE: THese are the texture_file entities
                if (!zox_has(texture, GenerateTexture)) {
                    continue;
                }
                if (zox_getv(texture, GenerateTexture)) {
                    if (dbg_log) {
                        zox_log("Tilemap still generating...");
                    }
                    still_generating = 1;
                    break;
                }
            //}
        }
        if (still_generating) {
            if (dbg_log) {
                zox_log("Tilemap Still Generating");
            }
            continue;
        }
        // generate size based on TilemapSize
        // entity first_texture = textures->value[0];
        if (!zox_valid(first_texture)) {
            zox_loge("Invalid First Texture for Tilemap");
            continue;
        }
        int2 unit_size = zox_getv(first_texture, TextureSize);
        // for example, 1 / 8 if size is 8
        // zox_log("Tilemap Size %i - u%i", map_size->value.x, unit_size.x);
        size->value.x = map_size->value.x * unit_size.x;
        size->value.y = map_size->value.y * unit_size.y;
        resize_TextureData(data, size->value.x * size->value.y);
        int2 texture_position = int2_zero;
        texture_index = 0;
        memset(data->value, 255, data->length * sizeof(color));
        while (1) {
            entity texture = zox_get_link_index(world, e, TextureLink, texture_index);
            if (!texture) {
                break;
            }
        /*iter it3 = zox_links(e, TextureLink);
        while (zox_query_next(it3)) {
            for (int j = 0; j < it3.count; j++) {
                entity texture = zox_pair_target(it3, j);*/
        /*for (texture_position.y = 0; texture_position.y < map_size->value.y && texture_index < textures->length; texture_position.y++) {
            for (texture_position.x = 0; texture_position.x < map_size->value.x && texture_index < textures->length; texture_position.x++) {
                entity texture = textures->value[texture_index];*/
                if (!zox_has(texture, TextureData)) {
                    zox_loge("invalid texture [%s] index [%i]", zox_get_name(texture), texture_index)
                    texture_index++;
                    continue;
                }
                zox_geter(texture, TextureData, texture_data);
                if (!texture_data->value) {
                    zox_loge("Invalid texture data [%s] index [%i]", zox_get_name(texture), texture_index);
                    texture_index++;
                    continue;
                }
                zox_geter_value(texture, TextureSize, int2, texture_size);
                texture_position.x =
                    texture_index % map_size->value.x;
                texture_position.y =
                    texture_index / map_size->value.x;
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
                        int pixel_index = int2_array_index(pixel_position, texture_size);
                        if (pixel_index >= texture_data->length) {
                            continue;
                        }
                        data->value[tilemap_index] = texture_data->value[pixel_index];
                    }
                }
                texture_index++;
            // }
        }
        generate->value = zox_generate_texture_uvs;
        zox_add(e, TextureDirty);
        if (dbg_log) {
            zox_log("Tilemap Generated!");
        }
    }
} zox_sys_end(TilemapGenerationSystem);

