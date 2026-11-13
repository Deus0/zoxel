

void clone_texture_data(ecs *world, entity e, entity src) {

    if (!src || !zox_has(src, TextureSize) || !zox_has(src, TextureData)) {
        if (!src) {
            zox_loge("[texture not found] [%s]", zox_get_name(e));
        } else {
            zox_loge("[texture invalid] [%s] > source [%s]", zox_get_name(e), zox_get_name(src));
        }
        return;
    }

    const TextureData* source_data = zox_get(src, TextureData);
    if (source_data->length == 0 || !source_data->value) {
        zox_loge("[clone_texture_data] Invalid src TextureData [%s] > source [%s]", zox_get_name(e), zox_get_name(src));
        return;
    }
    int2 size = zox_getv(src, TextureSize);
    const int bytes_length = sizeof(color) * source_data->length;

    TextureData data = { 0 };
    initialize_TextureData(&data, source_data->length);
    if (!data.value) {
        zox_loge("texture data malloc failed");
        return;
    }

    memcpy(data.value, source_data->value, bytes_length);
    zox_set_ptr(e, TextureData, data);
    zox_set(e, TextureSize, { size });
    zox_add(e, TextureDirty);

    // zox_log("cloned texture data %s => %s", zox_get_name(e), zox_get_name(src));
}
