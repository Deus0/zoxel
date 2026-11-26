/*
 * +--------------------------------------------------------------+
 * | Zox Module: Capsules                                         |
 * |                                                              |
 * |  A capsule's mesh: a cylinder with half spheres on its ends   |
 * |                                                              |
 * |  Uses: Rendering (lit meshes) - Colors                       |
 * |                                                              |
 * +--------------------------------------------------------------+
 *  - a capsule mesh is its shape as data: RadiusFloat, HeightFloat (its whole height, ends included), ColorRGB
 *  - spawning one adds Dirty, an observer builds its mesh (flat shaded, lit by the sun, its front darker); change
 *    its shape and add Dirty to build it again
 *  - zeland's character looks like one, its collider the same shape (Capsule, terrains/poly/collisions)
 */
zox_tag(CapsuleMesh);
#include "set/_.c"
#include "fun/mesh.c"
#include "pre/_.c"
#include "ins/_.c"
#include "eve/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_capsules(ecs* world) {
    zox_module(capsules);
    zoxd_tag(CapsuleMesh);
    add_hook_spawn_prefabs(spawn_prefabs_capsules);
    add_hook_spawn_prefabs(zox_events_capsules);
#ifdef zox_tests
    zox_tsts_capsules();
#endif
}
