entity material_bone;
entity material_bonet;
entity shader_basic3;
entity shader_basic3_overlay;
entity material_basic3D;

#include "clones.c"

void catch_basic3D_errors(char* function_name) {
#ifdef is_catch_basic3D_errors
    check_opengl_error(function_name);
#else
    (void) function_name;
#endif
}

void spawn_shaders_basics3D(ecs *world) {
    shader_basic3 = spawn_shader_basic3(world);
    material_basic3D = spawn_material_basic3D(world, shader_basic3);
    shader_basic3_overlay = spawn_shader_basic3_overlay(world);
    spawn_material_colored3D(world);
    spawn_material_textured3D(world);
#ifndef zox_disable_rendering_instances
    spawn_material_vox_instance(world);
#endif
    material_bone = spawn_material_bone(world, 0);
    material_bonet = spawn_material_bone(world, 1);
}
