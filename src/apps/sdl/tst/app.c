#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <android/log.h>
#include <stdarg.h>


static void zox_log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    __android_log_vprint(ANDROID_LOG_INFO, "Zoxel", format, args);
    va_end(args);
}

static void zox_loge(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    __android_log_vprint(ANDROID_LOG_ERROR, "Zoxel", format, args);
    va_end(args);
}

#define SDL_Log(...) zox_log(__VA_ARGS__)
#define SDL_LogError(category, ...) zox_loge(__VA_ARGS__)

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[TEST] SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Log("[TEST] SDL initialized");

    SDL_Window *window = SDL_CreateWindow(
        "Zoxel SDL Test",
        1280,
        720,
        0
    );

    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[TEST] SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Log("[TEST] Window created");

    int running = 1;

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }

        SDL_Delay(16);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
