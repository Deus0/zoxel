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
 *      - Spawn Sound
 *      - Destroy Sound
 *      - Scale Animation Curve
 *      - Child Particles
 *
 */
zox_tag(Projectile);
zox_tag(ProjectileUser);    // projectile -> who fired it, never hit by it
zox_tag(ProjectileSkill);   // projectile -> the skill that fired it
#include "pre/_.c"
#include "ins/_.c"
// #include "sys/_.c"

void import_projectiles(ecs* world) {
    zox_module(projectiles);
    zoxd_tag(Projectile);
    zoxd_nf_link(ProjectileUser);
    zoxd_nf_link(ProjectileSkill);
    // zox_systems_projectiles(world);
    add_hook_spawn_prefabs(zox_define_prefabs_projectiles);
}
