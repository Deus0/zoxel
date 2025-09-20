// todo: save properly
byte save_font_style(
    ecs *world,
    const entity e,
    char *resources_directory,
    char *filename
) {
    if (!e) {
        zox_log_error("error saving entity [%lu] invalid children [%s]", e, filename)
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
        zox_log_error(" > error saving [%s]", path)
        perror("Error opening file for writing");
        free(path);
        return 0;
    }

    // get full filepath
    // get binary for each font and added with seperation?
    zox_geter(e, Children, children);
    if (children == NULL) {
        zox_log_error("! error saving entity [%lu] invalid children [%s]", e, path)
        free(path);
        return 0;
    }

    SaveDataFontStyle data = {
        .length = children->length
    };
    zox_logv("   - font style children [%i]", children->length);
    for (int i = 0; i < children->length; i++) {
        const entity child = children->value[i];
        if (!child) {
            zox_logv(" > font [%i]", i);
            data.fonts[i] = (SaveDataFont) {
                .length = 0
            };
            continue;
        }

        zox_geter(child, FontData, fdata);
        zox_logv(" > font [%i] - points: [%i]", i, fdata->length);
        data.fonts[i] = (SaveDataFont) {
            .length = fdata->length
        };
        for (int j = 0; j < fdata->length; j++) {
            data.fonts[i].points[j] = fdata->value[j];
        }
    }

    fwrite(&data, sizeof(SaveDataFontStyle), 1, file);
    fclose(file);
    free(path);

    return 1; // success
}
