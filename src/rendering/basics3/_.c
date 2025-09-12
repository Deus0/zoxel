#ifndef zoxm_rendering_basics3
#define zoxm_rendering_basics3

// #define is_catch_basic3D_errors
zox_tag(MeshBasic3D);
zox_tag(TexturedMesh3D);
zox_tag(SkeletonMesh);
#include "dat/_.c"
#include "mat/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Rendering3)
    add_hook_load_shader(&spawn_shaders_basics3D);
    zoxd_tag(MeshBasic3D);
    zoxd_tag(TexturedMesh3D);
    zoxd_tag(SkeletonMesh);
    zoxd(MaterialTextured3D);
    zoxd(MaterialColored3D);
    zoxd(MaterialVoxInstance);
    zoxd(MaterialBasic3D);
    define_systems_basics3D(world);
zox_end_module(Rendering3)

#endif