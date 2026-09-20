#ifdef zox_web
// todo: move this to apps / viewports

// #define debug_ui_scaling
// extern int2 screen_dimensions;  // from apps/settings

// emscripten app functions (void)
int get_canvas_width() { return 1280; }
int get_canvas_height() { return 720; }
// EM_JS(int, get_canvas_width, (), { return window.innerWidth; });
// EM_JS(int, get_canvas_height, (), { return window.innerHeight; });

int2 get_canvas_size() {
    return (int2) { get_canvas_width(), get_canvas_height() };
}

// Condensed
void resize_canvas() {
    zox_log("   > resizing canvas [%ix%i]", get_canvas_width(), get_canvas_height())
}

int2 get_webasm_screen_size() {
    int2 canvas_size = get_canvas_size();
    zox_log("   > canvas screen dimensions [%ix%i]\n", canvas_size.x, canvas_size.y)
    // screen_dimensions = canvas_size;
    return canvas_size;
}

/*byte update_web_canvas(ecs *world) {
    int2 size = get_canvas_size();
    //if (!int2_equals(screen_dimensions, size)) {
        //zox_log(" > update_web_canvas: Canvas size has changed [%i x %i]\n", size.x, size.y)
        // set_viewport_size(world, (int2) { canvas_size.x, canvas_size.y });
    if (!int2_equals(size, zox_getv(main_app, WindowSize)) {
        zox_set(main_app, WindowSize, { size });
        zox_set(main_app, WindowSizeDirty, { zox_dirty_trigger });
        return 1;
    } else {
        return 0;
    }
    //}
}*/

extern int path_exists(const char* path);

byte initialize_pathing_web(
    const char** data_path,
    const char** resources_path,
    const char** resources_path_game)
{
    *data_path = "/";
    *resources_path = "/res";
    *resources_path_game = NULL;

    if (!path_exists(*resources_path)) {
        zox_loge("Resources Path did not exist [%s]",
            *resources_path);
        return EXIT_FAILURE;
    }

    zox_log("data_path %s", *data_path);
    zox_log("resources_path %s", *resources_path);

    return EXIT_SUCCESS;
}

#endif
