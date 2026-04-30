// Define the ComputeShaderLink component
zoxc_uint(ComputeShaderLink);

// Destructor for ComputeShaderLink component
ECS_DTOR(ComputeShaderLink, ptr, {
    if (ptr->value) glDeleteShader(ptr->value);
})

// Function to generate a compute shader on gpu
uint gpu_spawn_compute_shader() {
#ifndef zox_disable_compute
    uint buffer = create_shader_compute();
    return buffer;
#else
    return 0;
#endif
}

// Function to add a compute shader to a prefab
void prefab_add_compute_shader(ecs *world, const entity e) {
    if (!headless) zox_prefab_set(e, ComputeShaderLink, { 0 })
}

// Function to spawn and attach a compute shader to an entity
void spawn_compute_shader(ecs *world, const entity e) {
    if (!headless) zox_set(e, ComputeShaderLink, { gpu_spawn_compute_shader() })
}
