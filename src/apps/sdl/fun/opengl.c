// wrapper for SDL GL Swap Window
void app_update_gpu(ecs *world) {
    if (headless) {
        return;
    }
    if (!zox_valid(main_app)) {
        zox_log_error("[main_app] is missing");
        return;
    }
    zox_geter(main_app, SDLWindow, window)
    if (!window->value) {
        zox_log_error("[main_app] is has invalid SDLWindow");
        return;
    }
    SDL_GL_SwapWindow(window->value);
}
