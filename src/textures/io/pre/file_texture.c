entity spawn_texture_filepath(
    ecs *world,
    const entity prefab,
    const char *filepath
) {
    zox_instance(prefab)
    zox_name("texture_filepath")
    TextureData *textureData = &((TextureData) { 0, NULL });
    int2 texture_size = int2_zero;
    load_texture_from_png(filepath, textureData, &texture_size);
    if (!textureData->value) {
        zox_log_error("Load error [texture null] at [%s]", filepath);
        zox_delete(e)
        return 0;
    }
    zox_set(e, TextureData, { textureData->length, textureData->value });
    zox_set(e, TextureSize, { texture_size });
#ifdef zox_disable_io_textures
    zox_log(" ! texture io disabled at [%s]\n", filepath)
    zox_delete(e)
    return 0;
#endif
    return e;
}

entity spawn_texture_filename(
    ecs *world,
    char *filename
) {
    const entity source = string_hashmap_get(files_hashmap_textures, new_string_data(filename));
    zox_instance(prefab_texture);
    zox_name("texture_filename");
    clone_texture_data(world, e, source);
    return e;
}

void clone_texture_to_entity(
    ecs *world,
    const entity e,
    char *filename
) {
    const entity texture_source = string_hashmap_get(files_hashmap_textures, new_string_data(filename));
    if (!texture_source) {
        zox_log("! texture [%s] was not found", filename);
        return;
    }
    clone_texture_data(world, e, texture_source);
}

void clone_texture_entity_to_entity(
    ecs *world,
    const entity e,
    const entity texture_source
) {
    if (!texture_source) {
        zox_log("! texture [%lu] was invalid", texture_source)
        return;
    }
    clone_texture_data(world, e, texture_source);
}