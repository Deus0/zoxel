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

byte zox_app_get_monitor(ecs *world, entity e) {
    if (!zox_valid(e) || !zox_has(e, SDLWindow)) {
        return 0;
    }
    SDL_Window* sdl_window = zox_getv(e, SDLWindow);
    return zox_sdl_get_window_display(sdl_window);
}

ECS_DTOR(Context, ptr, {
    if (ptr->value != 0) {
        zox_sdl_gl_delete_context(ptr->value);
    }
})

#if defined(zox_sdl_mixer) && !defined(zox_sdl3)
    #include "mix_chunk.c"
#endif

void zox_components_sdl(ecs *world) {
    zoxd_dest_old(SDLWindow);
    zoxd_dest_old(Context);
    zoxd_dest_old(Renderer);
#if defined(zox_sdl_mixer) && !defined(zox_sdl3)
    zoxd_dest(SDLMixChunk);
#endif
}
