// save font style
#define directory_fonts "fonts"

void load_files_fonts(ecs *world) {

    // TODO: Load All Font Files as Styles
    // TODO: Create a Convert Module for TTF Library Imports

    const entity prefab = prefab_font_style;
    zox_font_style_default = spawn_font_style(world, prefab);
    // for now save
    // zox_font_style_monocraft is set in ttf initialization
    const byte loaded_ttf = initialize_ttf(world, prefab);
    if (loaded_ttf) {
        // save for now, testing
#if defined(zox_lib_ttf) && defined(convert_fonts)
        zox_log("+ Converting Font:\n")
        // "/home/deus/project/zoxel/resources/"
        save_font_style(world, zox_font_style_monocraft, resources_path, "new_font.zox");
#endif
    } else {
        zox_font_style_monocraft = load_font_style(world, "monocraft.zox");
    }
}
