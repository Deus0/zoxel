zoxc(SDLWindow, SDL_Window*);
zoxc(Renderer, SDL_Renderer*);
zoxc(Context, SDL_GLContext);

ECS_DTOR(SDLWindow, ptr, {
    if (ptr->value != 0) {
        SDL_DestroyWindow(ptr->value);
    }
})

ECS_DTOR(Renderer, ptr, {
    if (ptr->value != 0) {
        SDL_DestroyRenderer(ptr->value);
    }
})

ECS_DTOR(Context, ptr, {
    if (ptr->value != 0) {
        SDL_GL_DeleteContext(ptr->value);
    }
})

#include "mix_chunk.c"

void define_components_sdl(ecs *world) {
    zoxd_dest(SDLWindow);
    zoxd_dest(Context);
    zoxd_dest(Renderer);
    zoxd_dest(SDLMixChunk);
}
