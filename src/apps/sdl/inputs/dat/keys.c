/*#ifdef zox_sdl

    // #define definezoxkey(v, n) enum { zox_key_##v = SDLK_##v }

#else

    // #define definezoxkey(v1, v2) typedef zox_key_##v1 v2
    // #define definezoxkey(v1, v2) enum { zox_key_##v1 = v2 }
    #define definezoxkey(v, n) enum { zox_key_##v = n }

#endif*/

/* SDL keycode abstraction */

#define definezoxkey(V, v, n) enum { zox_key_##v = ZOX_SDL_KEYCODE_##V }
#define definezoxkey2(v, n) enum { zox_key_##v = ZOX_SDL_KEYCODE_##v }

definezoxkey(A, a, 0);
definezoxkey(B, b, 0);
definezoxkey(C, c, 0);
definezoxkey(D, d, 0);
definezoxkey(E, e, 0);
definezoxkey(F, f, 0);
definezoxkey(G, g, 0);
definezoxkey(H, h, 0);
definezoxkey(I, i, 0);
definezoxkey(J, j, 0);
definezoxkey(K, k, 0);
definezoxkey(L, l, 0);
definezoxkey(M, m, 0);
definezoxkey(N, n, 0);
definezoxkey(O, o, 0);
definezoxkey(P, p, 0);
definezoxkey(Q, q, 0);
definezoxkey(R, r, 0);
definezoxkey(S, s, 0);
definezoxkey(T, t, 0);
definezoxkey(U, u, 0);
definezoxkey(V, v, 0);
definezoxkey(W, w, 0);
definezoxkey(X, x, 0);
definezoxkey(Y, y, 0);
definezoxkey(Z, z, 0);

definezoxkey2(0, 0);
definezoxkey2(1, 0);
definezoxkey2(2, 0);
definezoxkey2(3, 0);
definezoxkey2(4, 0);
definezoxkey2(5, 0);
definezoxkey2(6, 0);
definezoxkey2(7, 0);
definezoxkey2(8, 0);
definezoxkey2(9, 0);

definezoxkey2(TAB, 0);

definezoxkey2(F1, 0);
definezoxkey2(F2, 0);
definezoxkey2(F3, 0);
definezoxkey2(F4, 0);
definezoxkey2(F5, 0);
definezoxkey2(F6, 0);
definezoxkey2(F7, 0);
definezoxkey2(F8, 0);
definezoxkey2(F9, 0);
definezoxkey2(F10, 0);
definezoxkey2(F11, 0);
definezoxkey2(F12, 0);
