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
    const byte* state = sdl_get_keyboard_state();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(Keyboard);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(Keyboard, keyboard);

#define zox_extract_key(key, sdl_scancode, sdl_key) \
        zox_set_key(world, &keyboard->key, state[sdl_scancode], sdl_key, dbg_log)

        zox_extract_key(space, ZOX_SDL_SCANCODE_SPACE, ZOX_SDL_KEYCODE_SPACE);
        zox_extract_key(back_quote, ZOX_SDL_SCANCODE_BACK_QUOTE, ZOX_SDL_KEYCODE_BACK_QUOTE);
        zox_extract_key(enter, ZOX_SDL_SCANCODE_ENTER, ZOX_SDL_KEYCODE_ENTER);
        zox_extract_key(escape, ZOX_SDL_SCANCODE_ESCAPE, ZOX_SDL_KEYCODE_ESCAPE);
        zox_extract_key(backspace, ZOX_SDL_SCANCODE_BACKSPACE, ZOX_SDL_KEYCODE_BACKSPACE);
        zox_extract_key(tab, ZOX_SDL_SCANCODE_TAB, ZOX_SDL_KEYCODE_TAB);

        zox_extract_key(left_control, ZOX_SDL_SCANCODE_LEFT_CONTROL, ZOX_SDL_KEYCODE_LEFT_CONTROL);
        zox_extract_key(right_control, ZOX_SDL_SCANCODE_RIGHT_CONTROL, ZOX_SDL_KEYCODE_RIGHT_CONTROL);
        zox_extract_key(left_alt, ZOX_SDL_SCANCODE_LEFT_ALT, ZOX_SDL_KEYCODE_LEFT_ALT);
        zox_extract_key(right_alt, ZOX_SDL_SCANCODE_RIGHT_ALT, ZOX_SDL_KEYCODE_RIGHT_ALT);
        zox_extract_key(left_shift, ZOX_SDL_SCANCODE_LEFT_SHIFT, ZOX_SDL_KEYCODE_LEFT_SHIFT);
        zox_extract_key(right_shift, ZOX_SDL_SCANCODE_RIGHT_SHIFT, ZOX_SDL_KEYCODE_RIGHT_SHIFT);

        zox_extract_key(up, ZOX_SDL_SCANCODE_UP, ZOX_SDL_KEYCODE_UP);
        zox_extract_key(down, ZOX_SDL_SCANCODE_DOWN, ZOX_SDL_KEYCODE_DOWN);
        zox_extract_key(left, ZOX_SDL_SCANCODE_LEFT, ZOX_SDL_KEYCODE_LEFT);
        zox_extract_key(right, ZOX_SDL_SCANCODE_RIGHT, ZOX_SDL_KEYCODE_RIGHT);

        zox_extract_key(a, ZOX_SDL_SCANCODE_A, ZOX_SDL_KEYCODE_A);
        zox_extract_key(b, ZOX_SDL_SCANCODE_B, ZOX_SDL_KEYCODE_B);
        zox_extract_key(c, ZOX_SDL_SCANCODE_C, ZOX_SDL_KEYCODE_C);
        zox_extract_key(d, ZOX_SDL_SCANCODE_D, ZOX_SDL_KEYCODE_D);
        zox_extract_key(e, ZOX_SDL_SCANCODE_E, ZOX_SDL_KEYCODE_E);
        zox_extract_key(f, ZOX_SDL_SCANCODE_F, ZOX_SDL_KEYCODE_F);
        zox_extract_key(g, ZOX_SDL_SCANCODE_G, ZOX_SDL_KEYCODE_G);
        zox_extract_key(h, ZOX_SDL_SCANCODE_H, ZOX_SDL_KEYCODE_H);
        zox_extract_key(i, ZOX_SDL_SCANCODE_I, ZOX_SDL_KEYCODE_I);
        zox_extract_key(j, ZOX_SDL_SCANCODE_J, ZOX_SDL_KEYCODE_J);
        zox_extract_key(k, ZOX_SDL_SCANCODE_K, ZOX_SDL_KEYCODE_K);
        zox_extract_key(l, ZOX_SDL_SCANCODE_L, ZOX_SDL_KEYCODE_L);
        zox_extract_key(m, ZOX_SDL_SCANCODE_M, ZOX_SDL_KEYCODE_M);
        zox_extract_key(n, ZOX_SDL_SCANCODE_N, ZOX_SDL_KEYCODE_N);
        zox_extract_key(o, ZOX_SDL_SCANCODE_O, ZOX_SDL_KEYCODE_O);
        zox_extract_key(p, ZOX_SDL_SCANCODE_P, ZOX_SDL_KEYCODE_P);
        zox_extract_key(q, ZOX_SDL_SCANCODE_Q, ZOX_SDL_KEYCODE_Q);
        zox_extract_key(r, ZOX_SDL_SCANCODE_R, ZOX_SDL_KEYCODE_R);
        zox_extract_key(s, ZOX_SDL_SCANCODE_S, ZOX_SDL_KEYCODE_S);
        zox_extract_key(t, ZOX_SDL_SCANCODE_T, ZOX_SDL_KEYCODE_T);
        zox_extract_key(u, ZOX_SDL_SCANCODE_U, ZOX_SDL_KEYCODE_U);
        zox_extract_key(v, ZOX_SDL_SCANCODE_V, ZOX_SDL_KEYCODE_V);
        zox_extract_key(w, ZOX_SDL_SCANCODE_W, ZOX_SDL_KEYCODE_W);
        zox_extract_key(x, ZOX_SDL_SCANCODE_X, ZOX_SDL_KEYCODE_X);
        zox_extract_key(y, ZOX_SDL_SCANCODE_Y, ZOX_SDL_KEYCODE_Y);
        zox_extract_key(z, ZOX_SDL_SCANCODE_Z, ZOX_SDL_KEYCODE_Z);

        zox_extract_key(f1, ZOX_SDL_SCANCODE_F1, ZOX_SDL_KEYCODE_F1);
        zox_extract_key(f2, ZOX_SDL_SCANCODE_F2, ZOX_SDL_KEYCODE_F2);
        zox_extract_key(f3, ZOX_SDL_SCANCODE_F3, ZOX_SDL_KEYCODE_F3);
        zox_extract_key(f4, ZOX_SDL_SCANCODE_F4, ZOX_SDL_KEYCODE_F4);
        zox_extract_key(f5, ZOX_SDL_SCANCODE_F5, ZOX_SDL_KEYCODE_F5);
        zox_extract_key(f6, ZOX_SDL_SCANCODE_F6, ZOX_SDL_KEYCODE_F6);
        zox_extract_key(f7, ZOX_SDL_SCANCODE_F7, ZOX_SDL_KEYCODE_F7);
        zox_extract_key(f8, ZOX_SDL_SCANCODE_F8, ZOX_SDL_KEYCODE_F8);
        zox_extract_key(f9, ZOX_SDL_SCANCODE_F9, ZOX_SDL_KEYCODE_F9);
        zox_extract_key(f10, ZOX_SDL_SCANCODE_F10, ZOX_SDL_KEYCODE_F10);
        zox_extract_key(f11, ZOX_SDL_SCANCODE_F11, ZOX_SDL_KEYCODE_F11);
        zox_extract_key(f12, ZOX_SDL_SCANCODE_F12, ZOX_SDL_KEYCODE_F12);

        zox_extract_key(_0, ZOX_SDL_SCANCODE_0, ZOX_SDL_KEYCODE_0);
        zox_extract_key(_1, ZOX_SDL_SCANCODE_1, ZOX_SDL_KEYCODE_1);
        zox_extract_key(_2, ZOX_SDL_SCANCODE_2, ZOX_SDL_KEYCODE_2);
        zox_extract_key(_3, ZOX_SDL_SCANCODE_3, ZOX_SDL_KEYCODE_3);
        zox_extract_key(_4, ZOX_SDL_SCANCODE_4, ZOX_SDL_KEYCODE_4);
        zox_extract_key(_5, ZOX_SDL_SCANCODE_5, ZOX_SDL_KEYCODE_5);
        zox_extract_key(_6, ZOX_SDL_SCANCODE_6, ZOX_SDL_KEYCODE_6);
        zox_extract_key(_7, ZOX_SDL_SCANCODE_7, ZOX_SDL_KEYCODE_7);
        zox_extract_key(_8, ZOX_SDL_SCANCODE_8, ZOX_SDL_KEYCODE_8);
        zox_extract_key(_9, ZOX_SDL_SCANCODE_9, ZOX_SDL_KEYCODE_9);
        zox_extract_key(_0, ZOX_SDL_SCANCODE_0, ZOX_SDL_KEYCODE_0);
    }
} zox_sys_end(KeyboardExtractSystem);
