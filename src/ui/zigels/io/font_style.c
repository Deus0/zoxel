entity spawn_font_style_save_data(ecs *world, entity prefab, const SaveDataFontStyle *data) {
    zox_logv("[spawn_font_style_save_data] fonts [%i]", data->length);
    entity prefab_font = zox_get_value(prefab, FontLink);
    zox_instance(prefab);
    zox_name("font_style_io");
    zox_add_tag(e, TTFFontStyle);
    for (int i = 0; i < data->length; i++) {
        if (i == zox_ascii_space) {
            // space
            spawn_font(world, e, prefab_font, font_space, font_space_length);
            continue;
        }
        if (!data->fonts[i].length) {
            spawn_font(world, e, prefab_font, font_question_mark, font_question_mark_length);
            continue;
        }
        spawn_font(world, e, prefab_font, data->fonts[i].points, data->fonts[i].length);
        // zox_logv(" - [%i] points: [%i]", i, data->fonts[i].length);
        /*if (i == 0) {
            for (int j = 0; j < data->fonts[i].length; j++) {
                zox_log("       - [0] point [%i] [%ix%i]\n", j, data->fonts[i].points[j].x, data->fonts[i].points[j].y)
            }
        }*/
    }
    return e;
}

entity load_font_style(ecs *world, char *filename) {
    // spawn font style and spawn fonts based on data
    char* directory = concat_file_path(resources_path, directory_fonts);
    char* directory_slash = concat_file_path(directory, character_slash);
    char* path = concat_file_path(directory_slash, filename);
    zox_logv("  - Loading Files Font [%s]", path);
    FILE *file = fopen(path, "rb");
    zox_logv(" + loading font style entity");
    zox_logv("   - to zox file [%s]", filename);
    zox_logv("   - directory [%s]", directory);
    zox_logv("   - full path [%s]", path);
    free(directory_slash);
    free(directory);
    if (!file) {
        zox_log_error(" > file is NULL at path [%s]", path)
        perror("Error opening file for writing");
        free(path);
        return 0;
    }
    zox_logv("   + success opening file");
    entity font_style = 0;
    SaveDataFontStyle data;
    size_t filesize = fread(&data, sizeof(SaveDataFontStyle), 1, file);
    if (filesize > 0) {
        font_style = spawn_font_style_save_data(
            world,
            prefab_font_style,
            &data
        );
    } else {
        zox_log_error("Filesize is 0 at [%s]", path);
        perror("filesize is 0");
    }
    fclose(file);
    free(path);
    return font_style;
}
