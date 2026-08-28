/**
 *  Shapes
 *
 *      - Simple data about shapes
 *      - Debugging tools to visualize them with lines
 *
 * */
#include "shapes3/_.c"

void import_shapes(ecs* world) {
    zox_module(shapes);
    zox_import_module(Shapes3);
}
