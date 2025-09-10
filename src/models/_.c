#ifndef zoxm_models
#define zoxm_models

// TODO: Refactor - Load Vox Files into ModelLinks

zox_tag(Model);
zox_tag(ModelCharacter);
#include "set/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Models)
    zoxd_tag(Model);
    zoxd_tag(ModelCharacter);
    zox_define_systems_models(world);
zox_end_module(Models)

#endif