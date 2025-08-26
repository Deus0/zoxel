#ifndef zoxm_rendering_basics3D
#define zoxm_rendering_basics3D

// #define is_catch_basic3D_errors
zox_tag(MeshBasic3D);
zox_tag(TexturedMesh3D);
zox_tag(SkeletonMesh);
#include "data/_.c"
#include "materials/_.c"
#include "util/_.c"
#include "systems/_.c"

zox_begin_module(Rendering3)
    zoxd_tag(MeshBasic3D);
    zoxd_tag(TexturedMesh3D);
    zoxd_tag(SkeletonMesh);
    zoxd(MaterialTextured3D);
    zoxd(MaterialColored3D);
    zoxd(MaterialVoxInstance);
    zoxd(MaterialBasic3D);
    define_systems_basics3D(world);
    add_hook_load_shader(&spawn_shaders_basics3D);
zox_end_module(Rendering3)

#endif