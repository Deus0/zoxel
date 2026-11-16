// uses terrain's texture links to generate a tilemap
// todo: support for multiple sizes, would have to place them in? or something
extern entity get_block_link(ecs*, entity);

// copies a texture's pixels into a tilemap at a pixel position
static inline void tilemap_blit(color* data, int2 size, int2 position, const color* pixels, int2 texture_size) {
    for (int y = 0; y < texture_size.y && position.y + y < size.y; y++) {
        for (int x = 0; x < texture_size.x && position.x + x < size.x; x++) {
            data[(position.y + y) * size.x + position.x + x] = pixels[y * texture_size.x + x];
        }
    }
}

// NOTE: Relies on texture size
void tilemap_generation_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TilemapSize);
    zox_sys_out(GenerateTexture);
    zox_sys_out(TextureSize);
    zox_sys_out(TextureData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TilemapSize, map_size);
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
        entity first_texture = 0;
        uint texture_index = 0;
        while (1) {
            entity texture = zox_get_link_index(
                world,
                e,
                TilemapTextureLinks,
                texture_index);
            texture_index++;
            if (!texture) {
                break;
            }
            if (!zox_has(texture, TextureData)) {
                zox_loge("Ivalid texture [%s] index [%i]",
                    zox_getn(texture),
                    texture_index);
                continue;
            }
            const TextureData* texture_data = zox_get(texture, TextureData);
            if (!texture_data->value) {
                zox_loge("Null TextureData [%s] index [%i]",
                    zox_getn(texture),
                    texture_index);
                still_generating = 1;
                break;
            }
            if (!first_texture) {
                first_texture = texture;
            }
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
        memset(data->value, 255, data->length * sizeof(color));
        texture_index = 0;
        while (1) {
            entity texture = zox_get_link_index(
                world,
                e,
                TilemapTextureLinks,
                texture_index);
            texture_index++;
            if (!texture) {
                break;
            }
            const TextureData* texture_data = zox_get(texture, TextureData);
            uint tilemap_index = texture_index - 1;
            int2 texture_size = zox_getv(texture, TextureSize);
            texture_position.x =
                tilemap_index % map_size->value.x;
            texture_position.y =
                tilemap_index / map_size->value.x;
            int2 tilemap_position = (int2) {
                texture_position.x * unit_size.x,
                texture_position.y * unit_size.y
            };
            if (texture_data->length < texture_size.x * texture_size.y) {
                continue;
            }
            tilemap_blit(data->value, size->value, tilemap_position, texture_data->value, texture_size);
        }
        generate->value = zox_generate_texture_uvs;
        zox_add(e, TextureDirty);
        if (dbg_log) {
            zox_log("Tilemap Generated!");
        }
    }
    zox_sys_on_end();
} zoxd_system(tilemap_generation_system);


/*iter it3 = zox_links(e, TextureLink);
 w hile (zox_query_next(it3)) {                             *
 for (int j = 0; j < it3.count; j++) {
     entity texture = zox_pair_target(it3, j);*/
/*for (texture_position.y = 0; texture_position.y < map_size->value.y && texture_index < textures->length; texture_position.y++) {
 f or (texture_position.x = 0; texture_position.x < map_*size->value.x && texture_index < textures->length; texture_position.x++) {
 entity texture = textures->value[texture_index];*/

