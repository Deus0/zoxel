void keydown_test_cubes(ecs *world, int32_t keycode) {
    if (keycode == SDLK_q) {
        zox_log("> spawning a cube")
        const entity e = spawn_cube(world, prefab_cube, float3_zero);
        zox_log("- cube exists now [%s]", zox_get_name(e))
    }
    if (keycode == SDLK_e) {
        zox_log("> spawning a cube_t")
        const entity e = spawn_cube_textured(world, prefab_cube_textured, float3_zero, 0);
        zox_log("- cube_t exists now [%s]", zox_get_name(e))
    }
}