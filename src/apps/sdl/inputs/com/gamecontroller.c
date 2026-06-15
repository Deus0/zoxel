#ifdef zox_sdl

zoxc(SdlGameController, SDL_GameController*);

ECS_DTOR(SdlGameController, ptr, {
    if (ptr->value) {
        SDL_GameControllerClose(ptr->value);
    }
})

#endif
