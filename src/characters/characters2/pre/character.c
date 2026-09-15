entity spawn_prefab_character2(ecs *world) {
    zox_prefab();
    zox_add(e, Character2D);
    // Physics
    zox_add(e, Frictioned);
    zox_setv(e, Position2, float2_zero);
    zox_setv(e, Rotation2, - 90 * degrees_to_radians);
    zox_setv(e, Scale1, 1);
    zox_setv(e, Velocity2D, 0);
    zox_setv(e, Acceleration2D, 0);
    zox_setv(e, Torque2D, 0);
    zox_setv(e, Bounds2D, float2_single(world_grid2D_size));
    // GPU
    zox_setv(e, Brightness, 1);
    zox_setv(e, MaterialGPULink, 0);
    zox_setv(e, TextureGPULink, 0);
    zox_setv(e, ElementLinks, 0);
    return e;
}
