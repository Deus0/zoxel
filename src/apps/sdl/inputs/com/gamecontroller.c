#ifdef zox_sdl

zoxc(SdlGameController, zox_sdl_gamepad*);

ECS_DTOR(SdlGameController, ptr, {
    if (ptr->value) {
        zox_sdl_gamepad_close(ptr->value);
    }
})

#endif
