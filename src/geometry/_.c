/*
 * +--------------------------------------------------------------+
 * | Zox Module: Geometry                                         |
 * |                                                              |
 * |  Cubes - Spheres - Planes - Cylinders - Lines                |
 * |                                                              |
 * |  Uses: Rendering - Transforms                                |
 * |                                                              |
 * +--------------------------------------------------------------+
 */
zox_tag(Cube);
zox_tag(Sphere);
zox_tag(Plane);
zox_tag(Cylinder);
#include "dat/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "dbg/_.c"
#include "lines/_.c"

void import_geometry(ecs* world) {
    zox_module(geometry);
    zoxd_tag(Cube);
    zoxd_tag(Sphere);
    zoxd_tag(Plane);
    zoxd_tag(Cylinder);
    add_hook_spawn_prefabs(zox_prefabs_geometry);
    zox_add_module(lines);
}
