
#ifdef zox_sdl

    #define definezoxkey(v, n) enum { zox_key_##v = SDLK_##v }
    // typedef zox_key_##v SDLK_##v

#else

    // #define definezoxkey(v1, v2) typedef zox_key_##v1 v2
    #define definezoxkey(v1, v2) enum { zox_key_##v1 = v2 }

#endif

definezoxkey(a, 0);
definezoxkey(b, 0);
definezoxkey(c, 0);
definezoxkey(d, 0);
definezoxkey(e, 0);
definezoxkey(f, 0);
definezoxkey(g, 0);
definezoxkey(h, 0);
definezoxkey(i, 0);
definezoxkey(j, 0);
definezoxkey(k, 0);
definezoxkey(l, 0);
definezoxkey(m, 0);
definezoxkey(n, 0);
definezoxkey(o, 0);
definezoxkey(p, 0);
definezoxkey(q, 0);
definezoxkey(r, 0);
definezoxkey(s, 0);
definezoxkey(t, 0);
definezoxkey(u, 0);
definezoxkey(v, 0);
definezoxkey(w, 0);
definezoxkey(x, 0);
definezoxkey(y, 0);
definezoxkey(z, 0);

definezoxkey(0, 0);
definezoxkey(1, 0);
definezoxkey(2, 0);
definezoxkey(3, 0);
definezoxkey(4, 0);
definezoxkey(9, 0);

definezoxkey(TAB, 0);

definezoxkey(F1, 0);
definezoxkey(F2, 0);
definezoxkey(F3, 0);
definezoxkey(F4, 0);
definezoxkey(F5, 0);
definezoxkey(F6, 0);
definezoxkey(F7, 0);
definezoxkey(F8, 0);
