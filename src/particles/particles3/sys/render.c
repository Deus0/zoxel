// #define zox_debug_particle3Ds

// TODO: Make particle type for lines, this looks coolaf
#ifdef zox_debug_particle3Ds
extern entity spawn_line3(ecs*, float3, float3, float, double);
#endif

void particle3d_render_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    const MaterialParticle3D* particle3D = zox_valid(material_particle3D) ? zox_get(material_particle3D, MaterialParticle3D) : NULL;
    if (!particle3D) {
        zox_sys_on_end();
        return;
    }
    zox_gpu_enable_points();
    zox_gpu_enable_blend();
    zox_gpu_material(zox_getv(material_particle3D, MaterialGPULink));
    zox_gpu_float4x4(particle3D->camera_matrix, render_camera_matrix);
    // a vox model's voxel in the world (vox_model_scale): its pixels at depth 1, the shader divides by the depth
    const float fov = (render_camera_fov ? render_camera_fov : 90) * (float) M_PI / 180.0f;
    zox_gpu_float(particle3D->thickness, vox_model_scale * render_viewport_height / (2 * tanf(fov / 2)));
    zox_sys_begin();
    zox_sys_in(Position3D);
    zox_sys_in(Color);
    zox_sys_in(Alpha);
    zox_sys_in(Scale1);
    zox_sys_in(Brightness);
    // each one's colour see through a little, and its fade (Alpha, fadeout_system)
    static Color colors[zox_max_particles3D];
    const int count = it->count < zox_max_particles3D ? it->count : zox_max_particles3D;
    for (int i = 0; i < count; i++) {
        colors[i] = Color_[i];
        colors[i].value.a = (byte) (colors[i].value.a * particle3_opacity * Alpha_[i].value);
    }
#if !defined(zox_disable_particles_gpu_instancing) && !defined(zox_disable_instancing)
    // position
    zox_gpu_bind_buffer_array(particle3D_instanced_position_buffer);
    zox_gpu_set_attribute_float3(particle3D->position, (void*) 0);
    zox_gpu_set_sub_buffer_float3(count, Position3D_);    // the capped count, as every attribute's
    zox_gpu_enable_attribute(particle3D->position);
    zox_gpu_attribute_divisor(particle3D->position, 1);
    // Color Data
    zox_gpu_bind_buffer_array(particle3D_instanced_color_buffer);
    zox_gpu_set_attribute_color(particle3D->color, (void*) 0);
    zox_gpu_set_sub_buffer_color(count, colors);
    zox_gpu_enable_attribute(particle3D->color);
    zox_gpu_attribute_divisor(particle3D->color, 1);
    // size, in voxels
    zox_gpu_bind_buffer_array(particle3D_instanced_size_buffer);
    zox_gpu_set_attribute_floatx(particle3D->size, (void*) 0, 1);
    zox_gpu_set_sub_buffer_bytes(count * sizeof(Scale1), Scale1_);
    zox_gpu_enable_attribute(particle3D->size);
    zox_gpu_attribute_divisor(particle3D->size, 1);
    // brightness: past 1 its colour goes past white, it glows (blooms, doc/plans/hdr.md)
    zox_gpu_bind_buffer_array(particle3D_instanced_brightness_buffer);
    zox_gpu_set_attribute_floatx(particle3D->brightness, (void*) 0, 1);
    zox_gpu_set_sub_buffer_bytes(count * sizeof(Brightness), Brightness_);
    zox_gpu_enable_attribute(particle3D->brightness);
    zox_gpu_attribute_divisor(particle3D->brightness, 1);
    // draw
    zox_gpu_render_points_instanced(count);
    // glDrawArraysInstanced(GL_POINTS, 0, 1, it->count);
    // resets
    zox_gpu_attribute_divisor(particle3D->color, 0); // Update per instance
    zox_gpu_attribute_divisor(particle3D->position, 0); // Update per instance
    zox_gpu_attribute_divisor(particle3D->size, 0);
    zox_gpu_attribute_divisor(particle3D->brightness, 0);
    zox_gpu_disable_attribute(particle3D->brightness);
    zox_gpu_disable_attribute(particle3D->size);
    zox_gpu_disable_attribute(particle3D->color);
    zox_gpu_disable_attribute(particle3D->position);
    zox_gpu_bind_buffer_array(0);
#else
    zox_gpu_enable_attribute(particle3D->position);
    zox_gpu_enable_attribute(particle3D->color);
    zox_gpu_enable_attribute(particle3D->size);
    zox_gpu_enable_attribute(particle3D->brightness);
    for (int i = 0; i < count; i++) {
        zox_sys_i(Position3D, position);
        zox_gpu_set_attribute_float3(particle3D->position, &position->value);
        zox_gpu_set_attribute_floatx(particle3D->size, &Scale1_[i].value, 1);
        zox_gpu_set_attribute_floatx(particle3D->brightness, &Brightness_[i].value, 1);
        zox_gpu_float4(particle3D->color, color_to_float4(colors[i].value));
        zox_gpu_render_points(1);
        // zox_log("Rendering Point at [%fx%fx%f]", position->value.x, position->value.y, position->value.z);
    }
    zox_gpu_disable_attribute(particle3D->brightness);
    zox_gpu_disable_attribute(particle3D->size);
    zox_gpu_disable_attribute(particle3D->color);
    zox_gpu_disable_attribute(particle3D->position);
#endif
    zox_disable_material();
    zox_gpu_disable_blend();
#ifdef zox_debug_particle3Ds
    float3 debug_particle_line_addition = (float3) { 0, 0.2f, 0 };
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Position3D, position);
        zox_sys_i(Color, color);
        float3 end = float3_add(position->value, debug_particle_line_addition);
        spawn_line3(world, position->value, end, 0.5f, 0.03);
    }
#endif
    // zox_log("Rendering Particles [%i]", it->count);
    zox_sys_on_end();
} zoxd_system(particle3d_render_system);


// todo: use one draw call with array of positions, instead of multiple calls here
// todo: also push in colors
// todo: test function that spawns 100k particles at once and lerps their positions or something out from origin


// const Rotation3D *rotation3D = &rotation3Ds[i];
// const Scale1 *scale1D = &scale1Ds[i];
// glVertexAttribPointer(particle3D->position, 3, GL_FLOAT, GL_FALSE, 0, position3D->value);
// zox_gpu_float3(particle3D->position, position3D->value);

    // zox_gpu_set_buffer_array(position3Ds. it->count * sizeof(float3));
    // glBufferData(GL_ARRAY_BUFFER, it->count * sizeof(float3), position3Ds, GL_STATIC_DRAW);
    // glVertexAttribPointer(particle3D->position, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    // glBufferSubData(GL_ARRAY_BUFFER, 0, it->count * sizeof(float3), Position3D_); //, GL_STATIC_DRAW);
    // Colors
    // glBufferData(GL_ARRAY_BUFFER, it->count * sizeof(color), colors, GL_STATIC_DRAW);
    // glVertexAttribPointer(particle3D->color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*) 0);
    // glBufferSubData(GL_ARRAY_BUFFER, 0, it->count * sizeof(color), Color_); // , GL_STATIC_DRAW);