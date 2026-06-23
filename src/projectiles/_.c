/*
 * Zox Projectiles
 *
 *      - Collides with stuff
 *
 * */
#ifndef zoxm_projectiles
#define zoxm_projectiles

// TODO: Add Vox Model option
// TODO: Add Collide with Terrain
// TODO: Add Spawn Sound and Destroy Sound
// TODO: Add a Scale Curve
// TODO: Add Particles on it

zox_tag(Projectile);
#include "pre/_.c"
#include "ins/_.c"
// #include "sys/_.c"

zox_begin_module(Projectiles) {
    zoxd_tag(Projectile);
    // zox_define_systems_projectiles(world);
    add_hook_spawn_prefabs(zox_define_prefabs_projectiles);
} zox_end_module(Projectiles);

#endif
