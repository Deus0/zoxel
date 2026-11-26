/*
 * +------------------------------------------------------+
 * | Zox Module: Poly Terrains                            |
 * |                                                      |
 * |  Heightmaps - Low Poly - Islands                     |
 * |                                                      |
 * +------------------------------------------------------+
 *
 *      - a grid of heights (PolyHeights), the surface its triangles draw (poly_height_at)
 *      - doc/plans/poly_terrains.md
 *
 * */

#include "set/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "dbg/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_poly_terrains(ecs* world) {
    zox_module(poly_terrains);
    zox_components_poly_terrains(world);
    add_hook_terminal_command(process_arguments_poly_terrains);
    add_hook_on_boot(poly_map_on_boot);
#ifdef zox_tests
    zox_tsts_poly_terrains();
#endif
}
