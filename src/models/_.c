/*
 * +------------------------------------------------------------------+
 * | Zox Module: Models                                               |
 * |                                                                  |
 * |  Meshes - Parts - Nodes - Generation                             |
 * |                                                                  |
 * +------------------------------------------------------------------+
 *
 *
 * Nodes for Processing Voxes
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
 *  TODO:
 *
 *      - Refactor old models into nodegraphs
 *      - Link Nodegraphs to realm - have them deleted after
 *      - Add Shape Information to fill
 *      - Create model size, which the octree can just divide higher for
 *      - Add Shape Type, Shape Size, to our Fill Nodes
 *      - Refactor: Load Vox Files into ModelLinks
 *      - Refactor: Vox Generation use Nodes / NodeGraphs
 *      - Can we Refactor ModelLinks from Rendering to here
 *
*/
#ifndef zoxm_models
#define zoxm_models

byte zox_block_outlines = 1;
#define zox_model_node_none 0
#define zox_model_node_colors 1
#define zox_model_node_fill 2
#define zox_model_node_paint 3
#define zox_model_node_size 4
#define zox_model_node_end 9

#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "gen/_.c"
#include "sys/_.c"
#include "generation/_.c"

zox_begin_module(Models) {
    define_components_models(world);
    define_systems_models(world);
    define_systems_models_generation(world);
    add_hook_spawn_prefabs(spawn_prefabs_models);
} zox_end_module(Models);

#endif
