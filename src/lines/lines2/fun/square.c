void spawn_line2D_square(ecs *world, float2 point, float2 size, float thickness, double life_time, color_rgb line_color) {
    spawn_line2D_colored(world,
        float2_add(point, (float2) { -size.x / 2.0f, -size.y / 2.0f }),
        float2_add(point, (float2) { size.x / 2.0f, -size.y / 2.0f }),
        thickness, life_time, line_color);
    spawn_line2D_colored(world,
        float2_add(point, (float2) { size.x / 2.0f, -size.y / 2.0f }),
        float2_add(point, (float2) { size.x / 2.0f, size.y / 2.0f }),
        thickness, life_time, line_color);
    spawn_line2D_colored(world,
        float2_add(point, (float2) { size.x / 2.0f, size.y / 2.0f }),
        float2_add(point, (float2) { -size.x / 2.0f, size.y / 2.0f }),
        thickness, life_time, line_color);
    spawn_line2D_colored(world,
        float2_add(point, (float2) { -size.x / 2.0f, size.y / 2.0f }),
        float2_add(point, (float2) { -size.x / 2.0f, -size.y / 2.0f }),
        thickness, life_time, line_color);
}
