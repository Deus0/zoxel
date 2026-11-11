// miracode | monocraft
// load its point data per character
// spawn that as a font style
// todo after: rewrite font_generation - a second system - to use shapes instead of just lines
// for now just load first one
#ifdef zox_lib_ttf

entity spawn_font_ttf(ecs *world, entity prefab_font, FT_Face face, float4 face_bounds, char charcode) {
    const FT_GlyphSlot glyph = get_glyph(face, charcode);
    if (!glyph) {
        return 0;
    }
    int length;
    byte2 *points = glyph_to_points(glyph, face_bounds, &length);
    if (!length) {
        return 0;
    }
    // fonts hold up to 255 points (a byte), more would wrap around
    if (length > 255) {
        zox_logw("font [%c] has [%i] points, keeping 255", charcode, length);
        length = 255;
    }
    entity e = spawn_font_direct(world, prefab_font, points, (byte) length);
    // free(points);
    return e;
}

entity spawn_ttf_as_font_style(ecs *world, entity prefab, FT_Face face) {
    entity prefab_font = zox_get_link(world, prefab, FontLink);
    float4 face_bounds = get_face_bounds(face);   // bounds used to calculate points
    // enter, options, exit
    zox_instance(prefab);
    zox_name("font_style_ttf");
    zox_add(e, TTFFontStyle);
    // glyphs are looked up by font index (convert_ascii), so spawns them in that order:
    //  index z draws the character convert_to_ascii(z), i.e. 0 is '?', 1 is 'a'
    //  (it used to spawn by ascii code, so most glyphs landed at the wrong index)
    for (int z = 0; z < font_styles_length; z++) {
        const char charcode = convert_to_ascii((byte) z);
        // unused indices map back to ' ', and a space has no outline
        const byte is_character = charcode >= 33 && charcode <= 126;
        const entity e2 = is_character ? spawn_font_ttf(world, prefab_font, face, face_bounds, charcode) : 0;
        if (e2) {
            zox_set_parent(world, e2, e);
        } else {
            spawn_font(world, e, prefab_font, font_question_mark, font_question_mark_length);
        }
    }
    return e;
}

entity spawn_ttf_path_as_font_style(ecs *world, entity prefab, const FT_Library *library, const char *load_path) {
    FT_Face face;
    if (FT_New_Face(*library, load_path, 0, &face)) {
        fprintf(stderr, " ! error: failure in initialize_ttf [%s]\n", load_path);
        return 0;
    }
#ifdef zox_print_files
    zox_log(" + loaded ttf [%s]\n", load_path)
#endif
    entity e = spawn_ttf_as_font_style(world, prefab, face);
    // print_glyph(get_glyph(face, '%'));
    FT_Done_Face(face);
    return e;
}

entity spawn_ttf_from_file(ecs *world, entity prefab, const FT_Library *library, const char *load_path) {
    // raw files sit beside res in the repo, the binary runs from bin/<build>/
    const char* raw_directory = raw_path ? raw_path : "../../raw";
    char* load_directory = concat_file_path(raw_directory, directory_fonts);
    char* load_directory_slash = concat_file_path(load_directory, character_slash);
    char* font_ttf = concat_file_path(load_directory_slash, load_path);
    zox_log("> loading [%s]\n", font_ttf)
    entity e = spawn_ttf_path_as_font_style(world, prefab, library, font_ttf);
    free(font_ttf);
    free(load_directory);
    free(load_directory_slash);
    return e;
}

byte initialize_ttf(ecs *world, entity prefab) {
    FT_Library library;
    if (FT_Init_FreeType(&library)) {
        fprintf(stderr, " ! Error: failure in initialize_ttf");
        return 0;
    }
    zox_log("+ loading [%s] from resources", default_font_ttf)
    zox_font_style_monocraft = spawn_ttf_from_file(world, prefab, &library, default_font_ttf);
    FT_Done_FreeType(library);
    return 1;
}

#else

byte initialize_ttf(ecs *world, const entity prefab) {
    (void) world;
    (void) prefab;
    // zox_log("! freetype is disabled\n")
    return 0;
}

#endif
