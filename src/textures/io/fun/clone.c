void clone_texture_file_to_entity(ecs *world, entity e, char *filename) {
    entity texture_source = string_hashmap_get(files_hashmap_textures, new_string_data(filename));
    if (!texture_source) {
        zox_log("! texture [%s] was not found", filename);
        return;
    }
    clone_texture_data(world, e, texture_source);
}
