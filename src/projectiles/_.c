/*
 * +------------------------------------------------------------------+
 * | Zox Module: Projectiles                                          |
 * |                                                                  |
 * |  Flight - Collision - Impact - Damage - Projectile State         |
 * |                                                                  |
 * +------------------------------------------------------------------+
 *
 *  TODO:
 *
 *      - Vox Model option
 *      - Collide with Terrain
 *      - Spawn Sound
 *      - Destroy Sound
 *      - Scale Animation Curve
 *      - Child Particles
 *
 */
zox_tag(Projectile);
#include "pre/_.c"
#include "ins/_.c"
// #include "sys/_.c"

void import_projectiles(ecs* world) {
    zox_module(projectiles);
    zoxd_tag(Projectile);
    // zox_define_systems_projectiles(world);
    add_hook_spawn_prefabs(zox_define_prefabs_projectiles);
}
