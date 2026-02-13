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
 *      Used By:
 *          - Terrain, Voxes (Blocks), Characters, Biomes
 *
*/
#ifndef zoxm_models
#define zoxm_models

// TODO: Refactor old models into nodegraphs
// TODO: Link Nodegraphs to realm - have them deleted after
// TODO: Add Shape Information to fill
// TODO: Create model size, which the octree can just divide higher for
// TODO: Add Shape Type, Shape Size, to our Fill Nodes

// TODO: Refactor: Load Vox Files into ModelLinks
// TODO: Refactor: Vox Generation use Nodes / NodeGraphs
// TODO: Can we Refactor ModelLinks from Rendering to here

#define zox_model_node_none 0
#define zox_model_node_colors 1
#define zox_model_node_fill 2
#define zox_model_node_paint 3

#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "gen/_.c"
#include "sys/_.c"

zox_begin_module(Models)
    define_components_models(world);
    define_systems_models(world);
    add_hook_spawn_prefabs(spawn_prefabs_models);
    initialize_settings_models(world);
zox_end_module(Models)

#endif
