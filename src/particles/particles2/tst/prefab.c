// tst: particle2_prefab, boot initializes the 2D prefab and instances keep independent state.
static byte zox_tst_particle2_ready(ecs* world) {
    zox_tst_waiting = "the 2D particle prefab";
    return zox_valid(prefab_particle2);
}

static byte zox_tst_particle2_defaults(ecs* world, entity particle) {
    return zox_has(particle, Particle) && zox_has(particle, Particle2D) &&
        zox_has(particle, Position2) && zox_has(particle, Rotation2) && zox_has(particle, Scale1) &&
        zox_has(particle, Velocity2D) && zox_has(particle, Acceleration2D) && zox_has(particle, Torque2D) &&
        zox_has(particle, Bounds2D) && zox_has(particle, Brightness) && zox_has(particle, DestroyInTime) &&
        float2_equals(zox_getv(particle, Position2), float2_zero) && zox_getv(particle, Rotation2) == 0 &&
        zox_getv(particle, Scale1) == 1 && float2_equals(zox_getv(particle, Velocity2D), float2_zero) &&
        float2_equals(zox_getv(particle, Acceleration2D), float2_zero) && zox_getv(particle, Torque2D) == 0 &&
        float2_equals(zox_getv(particle, Bounds2D), float2_zero) && zox_getv(particle, DestroyInTime) == 0;
}

static void zox_tst_particle2_prefab_check(ecs* world) {
    const entity first = zox_ins(world, prefab_particle2);
    const entity second = zox_ins(world, prefab_particle2);
    zox_tst_check(zox_tst_particle2_defaults(world, first), "the 2D particle instance had the wrong default state");
    zox_setv(first, Position2, ((float2) { 2, 3 }));
    zox_setv(first, Velocity2D, ((float2) { -1, 4 }));
    zox_setv(first, Scale1, 0.25f);
    zox_setv(first, DestroyInTime, 0.5);
    zox_tst_check(zox_tst_particle2_defaults(world, second), "changing one particle changed its sibling");
    zox_tst_check(zox_tst_particle2_defaults(world, prefab_particle2), "changing an instance changed the prefab");
    zox_tst_check(zox_tst_particle2_defaults(world, zox_ins(world, prefab_particle2)), "a fresh instance inherited another particle's state");
}
