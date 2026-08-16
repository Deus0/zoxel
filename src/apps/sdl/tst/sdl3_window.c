// cc src/apps/sdl/tst/sdl3_window.c $(pkg-config --cflags --libs sdl3) -lGL -o bin/sdl3_window_test && ./bin/sdl3_window_test
#include <SDL3/SDL.h>
#include <GL/gl.h>

int main(void) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init: %s", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow(
        "SDL3 OpenGL Decoration Test",
        800,
        600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        SDL_Log("SDL_CreateWindow: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Log("flags: %llx",
        (unsigned long long) SDL_GetWindowFlags(window));

    SDL_GLContext context = SDL_GL_CreateContext(window);

    if (!context) {
        SDL_Log("SDL_GL_CreateContext: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    if (!SDL_GL_MakeCurrent(window, context)) {
        SDL_Log("SDL_GL_MakeCurrent: %s", SDL_GetError());
        SDL_GL_DestroyContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Log("shown: %s",
        SDL_GetWindowFlags(window) & SDL_WINDOW_HIDDEN
            ? "no"
            : "yes");

    SDL_Log("window flags: %llx",
        (unsigned long long) SDL_GetWindowFlags(window));

    int x, y, w, h;

    if (SDL_GetWindowPosition(window, &x, &y)) {
        SDL_Log("position: %d %d", x, y);
    }

    if (SDL_GetWindowSize(window, &w, &h)) {
        SDL_Log("size: %d %d", w, h);
    }

    SDL_Log("GL: %s", (const char *) glGetString(GL_VERSION));
    SDL_Log("GLSL: %s", (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION));

    glViewport(0, 0, w, h);
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    SDL_GL_SwapWindow(window);

    SDL_MaximizeWindow(window);
    SDL_SyncWindow(window);
    SDL_Log("flags: %llx", (unsigned long long)SDL_GetWindowFlags(window));

    SDL_Event event;

    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            break;
        }

        if (event.type == SDL_EVENT_WINDOW_SHOWN) {
            SDL_Log("EVENT: WINDOW_SHOWN");
        } else if (event.type == SDL_EVENT_WINDOW_MOVED) {
            SDL_Log("EVENT: WINDOW_MOVED");
        } else if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
            SDL_Log(
                "EVENT: WINDOW_PIXEL_SIZE_CHANGED [%d, %d]",
                event.window.data1,
                event.window.data2
            );
        } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
            SDL_Log("RESIZED [%d, %d] flags [%llx]",
                event.window.data1,
                event.window.data2,
                (unsigned long long)SDL_GetWindowFlags(window));
        }
    }

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}