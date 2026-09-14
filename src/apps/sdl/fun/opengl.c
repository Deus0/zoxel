// wrapper for SDL GL Swap Window
void app_gpu_end_system(iter* it) {
    zox_sys_world();
    if (!zox_valid(main_app)) {
        zox_loge("[main_app] is missing");
        return;
    }
    if (!zox_has(main_app, SDLWindow)) {
        return;
    }
    zox_geter(main_app, SDLWindow, window);
    if (!window->value) {
        zox_loge("[main_app] is has invalid SDLWindow");
        return;
    }
    SDL_GL_SwapWindow(window->value);
} zoxd_system(app_gpu_end_system);
