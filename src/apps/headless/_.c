static inline int2 get_screen_size() { return int2_zero; }
void on_set_vsync(byte value) { }
void zox_set_app_fullscreen(ecs* world, entity e, byte fullscreen) { }
void zox_app_set_monitor_e(ecs *world, entity e, byte monitor) { }
void zox_set_app_maximized(ecs* world, entity e, byte maximized) { }
byte apps_is_extension_supported(const char* name) { return 0; }




/* SDL-compatible keycode values for headless builds.
 * ASCII is used for printable keys.
 * Function keys use SDL's extended keycode range.
 */

#define definezoxkey(V, v, n) enum { zox_key_##v = n }
#define definezoxkey2(v, n)   enum { zox_key_##v = n }

/* Letters */
definezoxkey(A, a, 'a');
definezoxkey(B, b, 'b');
definezoxkey(C, c, 'c');
definezoxkey(D, d, 'd');
definezoxkey(E, e, 'e');
definezoxkey(F, f, 'f');
definezoxkey(G, g, 'g');
definezoxkey(H, h, 'h');
definezoxkey(I, i, 'i');
definezoxkey(J, j, 'j');
definezoxkey(K, k, 'k');
definezoxkey(L, l, 'l');
definezoxkey(M, m, 'm');
definezoxkey(N, n, 'n');
definezoxkey(O, o, 'o');
definezoxkey(P, p, 'p');
definezoxkey(Q, q, 'q');
definezoxkey(R, r, 'r');
definezoxkey(S, s, 's');
definezoxkey(T, t, 't');
definezoxkey(U, u, 'u');
definezoxkey(V, v, 'v');
definezoxkey(W, w, 'w');
definezoxkey(X, x, 'x');
definezoxkey(Y, y, 'y');
definezoxkey(Z, z, 'z');

/* Numbers */
definezoxkey2(0, '0');
definezoxkey2(1, '1');
definezoxkey2(2, '2');
definezoxkey2(3, '3');
definezoxkey2(4, '4');
definezoxkey2(5, '5');
definezoxkey2(6, '6');
definezoxkey2(7, '7');
definezoxkey2(8, '8');
definezoxkey2(9, '9');

definezoxkey2(TAB, '\t');

/* SDL-compatible function key values */
definezoxkey2(F1,  1073741882);
definezoxkey2(F2,  1073741883);
definezoxkey2(F3,  1073741884);
definezoxkey2(F4,  1073741885);
definezoxkey2(F5,  1073741886);
definezoxkey2(F6,  1073741887);
definezoxkey2(F7,  1073741888);
definezoxkey2(F8,  1073741889);
definezoxkey2(F9,  1073741890);
definezoxkey2(F10, 1073741891);
definezoxkey2(F11, 1073741892);
definezoxkey2(F12, 1073741893);