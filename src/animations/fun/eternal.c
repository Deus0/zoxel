void add_eternal_euler(ecs* world, entity e, float3 euler) {
    zox_set(e, EternalRotation, { quaternion_from_euler( float3_scale(euler, degrees_to_radians)) });
    // (float3) { 0, 25 * degreesToRadians, 0 }) });
}
