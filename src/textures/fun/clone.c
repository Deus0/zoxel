

void clone_texture_data(ecs *world, entity e, entity src) {

    if (!src || !zox_has(src, TextureSize) || !zox_has(src, TextureData)) {
        if (!src) {
            zox_log_error("[texture not found] [%s]", zox_get_name(e));
        } else {
            zox_log_error("[texture invalid] [%s] > source [%s]", zox_get_name(e), zox_get_name(src));
        }
        return;
    }

    zox_geter(src, TextureData, source_data);
    if (source_data->length == 0 || !source_data->value) {
        zox_log_error("[clone_texture_data] Invalid src TextureData [%s] > source [%s]", zox_get_name(e), zox_get_name(src));
        return;
    }
    zox_geter_value(src, TextureSize, int2, size);
    const int bytes_length = sizeof(color) * source_data->length;

    TextureData data = { 0 };
    initialize_TextureData(&data, source_data->length);
    if (!data.value) {
        zox_log_error("texture data malloc failed");
        return;
    }

    memcpy(data.value, source_data->value, bytes_length);
    zox_set_ptr(e, TextureData, data);
    zox_set(e, TextureSize, { size });
    zox_add(e, TextureDirty);

    // zox_log("cloned texture data %s => %s", zox_get_name(e), zox_get_name(src));
}
