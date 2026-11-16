// animates a texture: its frames (TextureFrames) shown one at a time by the clock (TextureFrameRate), copied into its
//  TextureData and uploaded when the frame changes (water: its own gpu texture, liquid meshes draw it)
void texture_animate_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureSize);
    zox_sys_in(TextureFrames);
    zox_sys_in(TextureFrameRate);
    zox_sys_in(GenerateTexture);
    zox_sys_out(TextureFrame);
    zox_sys_out(TextureData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureSize, size);
        zox_sys_i(TextureFrames, frames);
        zox_sys_i(TextureFrameRate, rate);
        zox_sys_i(GenerateTexture, generate);
        zox_sys_o(TextureFrame, shown);
        zox_sys_o(TextureData, data);
        const int length = size->value.x * size->value.y;
        const int count = length ? frames->length / length : 0;
        if (generate->value || count < 2 || data->length != length) {
            continue;
        }
        const byte frame = texture_frame_at(zox_current_time, rate->value, count);
        if (frame == shown->value) {
            continue;
        }
        shown->value = frame;
        memcpy(data->value, &frames->value[frame * length], length * sizeof(color));
        zox_add(e, TextureDirty);
    }
    zox_sys_on_end();
} zoxd_system(texture_animate_system);
