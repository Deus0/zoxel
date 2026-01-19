/*
 * Models are Vox Blueprints
 *
 *      Nodes for Processing Voxes
 *
 *      - Fill Shape
 *      - Add Shape
 *      - Remove Shape
 *      - Outline
 *      - Add Noise
 *
*/
#ifndef zoxm_models
#define zoxm_models

// TODO: Refactor - Load Vox Files into ModelLinks

#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Models)
    define_components_models(world);
    define_systems_models(world);
zox_end_module(Models)

#endif