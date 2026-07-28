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

#ifdef zox_sdl_mixer
    #include "mix_chunk.c"
#endif

void define_components_sdl(ecs *world) {
    zoxd_dest_old(SDLWindow);
    zoxd_dest_old(Context);
    zoxd_dest_old(Renderer);
#ifdef zox_sdl_mixer
    zoxd_dest(SDLMixChunk);
#endif
}
