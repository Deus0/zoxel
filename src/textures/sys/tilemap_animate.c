// animates a tilemap: its animated textures' tiles (TilemapAnimations) show their frame for the time, then it uploads
//  - a frame is picked from the clock (TextureFrameRate), nothing counts down
void tilemap_animate_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureSize);
    zox_sys_in(GenerateTexture);
    zox_sys_out(TilemapAnimations);
    zox_sys_out(TextureData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureSize, size);
        zox_sys_i(GenerateTexture, generate);
        zox_sys_o(TilemapAnimations, animations);
        zox_sys_o(TextureData, data);
        if (generate->value || !data->length) {
            continue;
        }
        byte changed = 0;
        for (int j = 0; j < animations->length; j++) {
            TilemapAnimation* animation = &animations->value[j];
            const entity texture = animation->texture;
            if (!zox_valid(texture) || !zox_has(texture, TextureFrames)) {
                continue;
            }
            const TextureFrames* frames = zox_get(texture, TextureFrames);
            const int2 texture_size = zox_getv(texture, TextureSize);
            const int frame_length = texture_size.x * texture_size.y;
            const int count = frame_length ? frames->length / frame_length : 0;
            if (count < 2) {
                continue;
            }
            const byte frame = texture_frame_at(zox_current_time, zox_getv(texture, TextureFrameRate), count);
            if (frame == animation->frame) {
                continue;
            }
            animation->frame = frame;
            tilemap_blit(data->value, size->value, animation->position, &frames->value[frame * frame_length], texture_size);
            changed = 1;
        }
        if (changed) {
            zox_add(e, TextureDirty);
        }
    }
    zox_sys_on_end();
} zoxd_system(tilemap_animate_system);
