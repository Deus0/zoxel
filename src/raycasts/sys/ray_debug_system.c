extern void render_line3D(ecs *world, const float3, const float3, const color_rgb);

void RayDebugSystem(iter *it) {
    const color_rgb debug_color = { 255, 55, 55 };
    const color_rgb debug_color2 = { 155, 155, 155 };
    const float ray_length = 16;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RaycastOrigin);
    zox_sys_in(RaycastNormal);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RaycastOrigin, origin);
        zox_sys_i(RaycastNormal, normal);

        float3 end_point = float3_scale(normal->value, ray_length);
        float3 end_point2 = float3_scale(float3_up, 1);
        render_line3D(world, origin->value, float3_add(origin->value, end_point), debug_color);
        render_line3D(world, origin->value, float3_add(origin->value, end_point2), debug_color2);
    }
} zoxd_system2(RayDebugSystem);