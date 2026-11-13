// a path here saves the next frame to it, then clears (i.e. tests looking at the rendering)
char zox_screenshot_request[256] = "";

// wrapper for SDL GL Swap Window
void app_gpu_end_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    if (!zox_valid(main_app)) {
        zox_loge("[main_app] is missing");
        return;
    }
    if (!zox_has(main_app, SDLWindow)) {
        return;
    }
    const SDLWindow* window = zox_get(main_app, SDLWindow);
    if (!window->value) {
        zox_loge("[main_app] is has invalid SDLWindow");
        return;
    }
    if (zox_screenshot_request[0]) {
        if (zox_gpu_save_screenshot(zox_screenshot_request)) {
            zox_log("+ saved screenshot [%s]", zox_screenshot_request);
        } else {
            zox_loge("failed to save screenshot [%s]", zox_screenshot_request);
        }
        zox_screenshot_request[0] = 0;
    }
    SDL_GL_SwapWindow(window->value);
    if (background) {
        // a hidden window is never presented, so the gpu driver (mali on wayland) never
        // retires frames and keeps every freed/re-uploaded buffer, ~100MB/s until the pc freezes
        glFinish();
    }
    zox_sys_on_end();
} zoxd_system(app_gpu_end_system);
