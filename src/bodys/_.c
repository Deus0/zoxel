/*
 * Bodys are Combining Voxes
 *
 *      - Items
 *      - Body UI
 *
 *      Used By:
 *          - Characters
 *
*/
#ifndef zoxm_bodys
#define zoxm_bodys

// #define zox_debug_head_only

#define body_anchor_core 0
#define body_anchor_top 1
#define body_anchor_bottom 2
#define body_anchor_right 3
#define body_anchor_left 4
#define body_anchor_back 5
#define body_anchor_forward 6

// #include "set/_.c"
#include "com/_.c"
#include "sta/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Bodys) {
    define_components_bodys(world);
    define_systems_bodys(world);
    add_hook_spawn_prefabs(spawn_prefabs_bodys);
    // initialize_settings_models(world);
    add_taskbar_button((hook_taskbar) {
        .index = 6,
        .spawn = &spawn_player_menu_body,
        .component_id = MenuBody,
        .texture_name = "taskbar_body",
        .tooltip_text = "Body"
    });
} zox_end_module(Bodys)

// TODO: make use TargetChunkLod -> and load/unload depth based on that
// TODO: set lod here instead of just applying RenderDepth only with ChunkMeshDirty -> keeps memory down
// TODO: Support for multi chunk voxes


#endif
