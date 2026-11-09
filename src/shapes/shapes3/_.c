/**
 *  Shapes3
 *
 *      - Simple data about shapes
 *      - Debugging tools to visualize them with lines
 *
 * */
#include "com/_.c"

void import_shapes3(ecs* world) {
    zox_module(shapes3);
    zox_components_shapes3(world);
}
