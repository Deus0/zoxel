void zox_set_key(ecs *world, PhysicalButton *key, byte key_down, SDL_Keycode keycode, byte dbg_log) {
    if (!key->is_pressed && key_down) {
        key->pressed_this_frame = 1;
        run_hook_key_down(world, (int32_t) keycode);
        if (dbg_log) {
            zox_log("Key Down [%i]", keycode);
        }
    }
    if (key->is_pressed && !key_down) {
        key->released_this_frame = 1;
    }
    key->is_pressed = key_down;
}

zox_sys2(KeyboardExtractSystem) {
    byte dbg_log = 0;
    const Uint8* state = SDL_GetKeyboardState(NULL);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(Keyboard);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Keyboard, keyboard);
        #define zox_extract_key(key, sdl_scancode, sdl_key) \
            zox_set_key(world, &keyboard->key, state[sdl_scancode], sdl_key, dbg_log)
        zox_extract_key(space,         SDL_SCANCODE_SPACE,      SDLK_SPACE);
        zox_extract_key(back_quote,    SDL_SCANCODE_GRAVE,      SDLK_BACKQUOTE);
        zox_extract_key(enter,         SDL_SCANCODE_RETURN,     SDLK_RETURN);
        zox_extract_key(escape,        SDL_SCANCODE_ESCAPE,     SDLK_ESCAPE);
        zox_extract_key(backspace,     SDL_SCANCODE_BACKSPACE,  SDLK_BACKSPACE);
        zox_extract_key(tab,           SDL_SCANCODE_TAB,        SDLK_TAB);

        zox_extract_key(left_control,  SDL_SCANCODE_LCTRL,      SDLK_LCTRL);
        zox_extract_key(right_control, SDL_SCANCODE_RCTRL,      SDLK_RCTRL);
        zox_extract_key(left_alt,      SDL_SCANCODE_LALT,       SDLK_LALT);
        zox_extract_key(right_alt,     SDL_SCANCODE_RALT,       SDLK_RALT);
        zox_extract_key(left_shift,    SDL_SCANCODE_LSHIFT,     SDLK_LSHIFT);
        zox_extract_key(right_shift,   SDL_SCANCODE_RSHIFT,     SDLK_RSHIFT);
        zox_extract_key(up,            SDL_SCANCODE_UP,         SDLK_UP);
        zox_extract_key(down,          SDL_SCANCODE_DOWN,       SDLK_DOWN);
        zox_extract_key(left,          SDL_SCANCODE_LEFT,       SDLK_LEFT);
        zox_extract_key(right,         SDL_SCANCODE_RIGHT,      SDLK_RIGHT);
        zox_extract_key(a, SDL_SCANCODE_A, SDLK_a);
        zox_extract_key(b, SDL_SCANCODE_B, SDLK_b);
        zox_extract_key(c, SDL_SCANCODE_C, SDLK_c);
        zox_extract_key(d, SDL_SCANCODE_D, SDLK_d);
        zox_extract_key(e, SDL_SCANCODE_E, SDLK_e);
        zox_extract_key(f, SDL_SCANCODE_F, SDLK_f);
        zox_extract_key(g, SDL_SCANCODE_G, SDLK_g);
        zox_extract_key(h, SDL_SCANCODE_H, SDLK_h);
        zox_extract_key(i, SDL_SCANCODE_I, SDLK_i);
        zox_extract_key(j, SDL_SCANCODE_J, SDLK_j);
        zox_extract_key(k, SDL_SCANCODE_K, SDLK_k);
        zox_extract_key(l, SDL_SCANCODE_L, SDLK_l);
        zox_extract_key(m, SDL_SCANCODE_M, SDLK_m);
        zox_extract_key(n, SDL_SCANCODE_N, SDLK_n);
        zox_extract_key(o, SDL_SCANCODE_O, SDLK_o);
        zox_extract_key(p, SDL_SCANCODE_P, SDLK_p);
        zox_extract_key(q, SDL_SCANCODE_Q, SDLK_q);
        zox_extract_key(r, SDL_SCANCODE_R, SDLK_r);
        zox_extract_key(s, SDL_SCANCODE_S, SDLK_s);
        zox_extract_key(t, SDL_SCANCODE_T, SDLK_t);
        zox_extract_key(u, SDL_SCANCODE_U, SDLK_u);
        zox_extract_key(v, SDL_SCANCODE_V, SDLK_v);
        zox_extract_key(w, SDL_SCANCODE_W, SDLK_w);
        zox_extract_key(x, SDL_SCANCODE_X, SDLK_x);
        zox_extract_key(y, SDL_SCANCODE_Y, SDLK_y);
        zox_extract_key(z, SDL_SCANCODE_Z, SDLK_z);
        zox_extract_key(f1,  SDL_SCANCODE_F1,  SDLK_F1);
        zox_extract_key(f2,  SDL_SCANCODE_F2,  SDLK_F2);
        zox_extract_key(f3,  SDL_SCANCODE_F3,  SDLK_F3);
        zox_extract_key(f4,  SDL_SCANCODE_F4,  SDLK_F4);
        zox_extract_key(f5,  SDL_SCANCODE_F5,  SDLK_F5);
        zox_extract_key(f6,  SDL_SCANCODE_F6,  SDLK_F6);
        zox_extract_key(f7,  SDL_SCANCODE_F7,  SDLK_F7);
        zox_extract_key(f8,  SDL_SCANCODE_F8,  SDLK_F8);
        zox_extract_key(f9,  SDL_SCANCODE_F9,  SDLK_F9);
        zox_extract_key(f10, SDL_SCANCODE_F10, SDLK_F10);
        zox_extract_key(f11, SDL_SCANCODE_F11, SDLK_F11);
        zox_extract_key(f12, SDL_SCANCODE_F12, SDLK_F12);
        zox_extract_key(_1, SDL_SCANCODE_1, SDLK_1);
        zox_extract_key(_2, SDL_SCANCODE_2, SDLK_2);
        zox_extract_key(_3, SDL_SCANCODE_3, SDLK_3);
        zox_extract_key(_4, SDL_SCANCODE_4, SDLK_4);
        zox_extract_key(_5, SDL_SCANCODE_5, SDLK_5);
        zox_extract_key(_6, SDL_SCANCODE_6, SDLK_6);
        zox_extract_key(_7, SDL_SCANCODE_7, SDLK_7);
        zox_extract_key(_8, SDL_SCANCODE_8, SDLK_8);
        zox_extract_key(_9, SDL_SCANCODE_9, SDLK_9);
        zox_extract_key(_0, SDL_SCANCODE_0, SDLK_0);
    }
} zox_sys_end(KeyboardExtractSystem);
