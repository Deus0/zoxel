uint2 particle3D_shader;
uint particle3D_material;
uint particle3D_position_location;
uint particle3D_color_location;
uint particle3D_camera_matrix_location;
uint particle3D_fog_data_location;
uint particle3D_location_thickness;
uint particle3D_instanced_position_buffer;
uint particle3D_instanced_color_buffer;

void initialize_particle_gpu_instancing(uint particle3D_position_location, uint particle3D_color_location, int max_particles) {
#ifdef zox_disable_particles_gpu_instancing
    return;
#endif
    // zox_log(" > locations are: %i x %i\n", particle3D_position_location, particle3D_color_location)
    // Setup position buffer
    particle3D_instanced_position_buffer = zox_gpu_create_buffer();
    // glGenBuffers(1, &particle3D_instanced_position_buffer);
    zox_gpu_bind_buffer_array(particle3D_instanced_position_buffer);

    // glBufferData(GL_ARRAY_BUFFER, sizeof(Position3D) * max_particles, NULL, GL_STATIC_DRAW); // NULL for data to be uploaded later
    zox_gpu_set_buffer_array(NULL, sizeof(Position3D) * max_particles);

    zox_gpu_set_attribute_float3(particle3D_position_location, (void*) 0);
    // glVertexAttribPointer(particle3D_position_location, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);

    // glEnableVertexAttribArray(particle3D_position_location);
    // glVertexAttribPointer(particle3D_position_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // glVertexAttribDivisor(particle3D_position_location, 1); // Update per instance
    // glDisableVertexAttribArray(particle3D_position_location);

    // Setup color buffer
    particle3D_instanced_color_buffer = zox_gpu_create_buffer();
    // glGenBuffers(1, &particle3D_instanced_color_buffer);
    zox_gpu_bind_buffer_array(particle3D_instanced_color_buffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(Color) * max_particles, NULL, GL_STATIC_DRAW); // NULL for data to be uploaded later
    zox_gpu_set_buffer_array(NULL, sizeof(Color) * max_particles);

    zox_gpu_set_attribute_color(particle3D_color_location, (void*) 0);
    // glVertexAttribPointer(particle3D_color_location, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*) 0);
    // glEnableVertexAttribArray(particle3D_color_location);
    // glVertexAttribPointer(particle3D_color_location, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)0);
    // glVertexAttribDivisor(particle3D_color_location, 1); // Update per instance
    // glDisableVertexAttribArray(particle3D_color_location);
    // Unbind the buffer
    zox_gpu_bind_buffer_array(0);
}

void cleanup_particle_gpu_instancing() {
#ifdef zox_disable_particles_gpu_instancing
    return;
#endif
    zox_gpu_dispose_buffer(particle3D_instanced_position_buffer);
    zox_gpu_dispose_buffer(particle3D_instanced_color_buffer);
}

int initialize_shader_particle3D(ecs *world) {
    char* vert = get_shader_source(world, "particle3D.vert");
    char* frag = get_shader_source(world, "particle3D.frag");
    particle3D_shader = zox_gpu_compile_shader(vert, frag);
    if (uint2_equals(particle3D_shader, uint2_zero)) {
        zox_log_error("particle3D_shader has failed")
        return EXIT_FAILURE;
    }
    particle3D_material = spawn_gpu_material_program((const uint2) { particle3D_shader.x, particle3D_shader.y });
    if (!particle3D_material) {
        zox_log_error("=> [initialize_shader_particle3D] Failed:\n%s", vert);
        return EXIT_FAILURE;
    }
    particle3D_position_location = zox_gpu_get_material_attribute(particle3D_material, "position");
    particle3D_color_location = zox_gpu_get_material_attribute(particle3D_material, "color");
    particle3D_camera_matrix_location = zox_gpu_get_material_property(particle3D_material, "camera_matrix");
    particle3D_fog_data_location = zox_gpu_get_material_property(particle3D_material, "fog_data");
    particle3D_location_thickness = zox_gpu_get_material_property(particle3D_material, "thickness");
    initialize_particle_gpu_instancing(particle3D_position_location, particle3D_color_location, zox_max_particles3D);

/*#ifndef zox_gles2
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif*/

    return EXIT_SUCCESS;
}

void dispose_shader_particle3D() {
    zox_gpu_dispose_shader(particle3D_shader.x);
    zox_gpu_dispose_shader(particle3D_shader.y);
    zox_dispose_material(particle3D_material);
    cleanup_particle_gpu_instancing();
}
