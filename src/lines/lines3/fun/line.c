void debug_line(ecs *world, const float3 a, const float3 b) {
    spawn_line3c(world, a, b, 1, 0.03f, color_rgb_red);
}

void debug_linec(ecs *world, const float3 a, const float3 b, color_rgb c) {
    spawn_line3c(world, a, b, 1, 0.03f, c);
}
