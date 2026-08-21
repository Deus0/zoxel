void spawn_arrow3D(ecs *world, float3 point, float3 normal, float arrow_radius, float thickness, double life_time) {
    float3 pointB = float3_add(point, normal);
    spawn_line3(world, point, pointB, thickness, life_time);
    float3 pointC = float3_subtract(point, float3_scale(normal, -0.1f));
    // todo: move arrow_radius along normal axis
    spawn_line3(world, point, float3_add(pointC, (float3) { arrow_radius, 0, 0 }), thickness, life_time);
    spawn_line3(world, point, float3_add(pointC, (float3) { -arrow_radius, 0, 0 }), thickness, life_time);
    spawn_line3(world, point, float3_add(pointC, (float3) { 0, 0, arrow_radius }), thickness, life_time);
    spawn_line3(world, point, float3_add(pointC, (float3) { 0, 0, -arrow_radius }), thickness, life_time);
}
