const char *outputTextureName = resources_folder_name"textures/tester.png"; // testing

zox_sys2(TextureSaveSystem) {
    zox_skip_if_unchanged(1)
    // if (!ecs_query_changed(NULL, it)) return;
    zox_field_in(TextureData, textures, 2)
    zox_field_in(TextureSize, textureSizes, 3)
    for (int i = 0; i < it->count; i++) {
        const TextureData *textureData = &textures[i];
        const TextureSize *textureSize = &textureSizes[i];
        save_texture_as_png(textureData->value, textureSize->value, outputTextureName);
        zox_log(" > saved texture: %lu as [%s]\n", (long int) it->entities[i], outputTextureName)
    }
} zox_sys_end(TextureSaveSystem);