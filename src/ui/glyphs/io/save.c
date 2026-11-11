// Saves a font style's glyph outlines as a .zox (see bsh/convert.sh --fonts)
byte save_font_style(ecs *world, entity e, char *resources_directory, char *filename) {
    if (!e) {
        zox_loge("error saving entity [%lu] invalid children [%s]", e, filename)
        return 0;
    }
    char* directory = concat_file_path(resources_directory, directory_fonts);
    char* directory2 = concat_file_path(directory, character_slash);
    char* path = concat_file_path(directory2, filename);
    zox_logv(" + saving font style entity [%s]", zox_get_name(e));
    zox_logv("   - to zox file [%s]", filename);
    zox_logv("   - directory [%s]", directory);
    zox_logv("   - full path [%s]", path);
    free(directory);
    free(directory2);
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        zox_loge(" > error saving [%s]", path)
        perror("Error opening file for writing");
        free(path);
        return 0;
    }
    // get full filepath
    // get binary for each font and added with seperation?
    // fonts in child order, the order glyphs are looked up by (zox_get_children, see font_texture.c)
    entity children[max_save_data_fonts];
    const uint children_length = zox_get_children(world, e, children, max_save_data_fonts);
    static SaveDataFontStyle data;
    memset(&data, 0, sizeof(data));
    data.length = children_length;
    zox_logv("   - font style children [%i]", children_length);
    for (uint i = 0; i < children_length; i++) {
        const entity child = children[i];
        if (!zox_valid(child) || !zox_has(child, FontData)) {
            continue;
        }
        zox_geter(child, FontData, fdata);
        const int length = fdata->length < max_font_points ? fdata->length : max_font_points;
        if (fdata->length > max_font_points) {
            zox_logw("font [%i] has [%i] points, saving [%i]", i, fdata->length, max_font_points);
        }
        data.fonts[i].length = length;
        for (int j = 0; j < length; j++) {
            data.fonts[i].points[j] = fdata->value[j];
        }
    }
    fwrite(&data, sizeof(SaveDataFontStyle), 1, file);
    fclose(file);
    free(path);
    return 1; // success
}
