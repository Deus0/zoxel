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
    zox_geter_value_non_const(e, SDLWindow, SDL_Window*, sdl_window)
    return zox_sdl_get_window_display(sdl_window);
}

ECS_DTOR(Context, ptr, {
    if (ptr->value != 0) {
        zox_sdl_gl_delete_context(ptr->value);
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
