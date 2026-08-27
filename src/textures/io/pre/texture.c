entity spawn_texture_filepath(ecs *world, entity prefab, const char *path) {
    zox_instance(prefab);
    zox_name("texture_filepath");
    TextureData texture = (TextureData) { 0 };
    int2 size = int2_zero;
    if (ends_with(path, ".bmp") || ends_with(path, ".BMP")) {
        load_texture_from_bmp(path, &texture, &size);
    } else if (ends_with(path, ".png") || ends_with(path, ".PNG")) {
        load_texture_from_png(path, &texture, &size);
    } else {
        zox_loge("File type unsupported [%s]", path);
        zox_delete(e);
        return 0;
    }
    if (!texture.value) {
        // zox_logw("Texture failed to load [%s]", path);
        zox_delete(e);
        return 0;
    }
    zox_set_ptr(e, TextureData, texture);
    zox_set(e, TextureSize, { size });
    return e;
}

entity spawn_texture_filename(
    ecs *world,
    char *filename,
    int2 size)
{
    entity source = string_hashmap_get(files_hashmap_textures, new_string_data(filename));
    zox_instance(prefab_texture);
    zox_name("texture_filename");
    clone_texture_data_scale(world, e, source, size);
    return e;
}
