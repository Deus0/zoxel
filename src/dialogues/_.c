#ifndef zoxm_dialogues
#define zoxm_dialogues

// TODO: NodeTree prefab as DialogueTree
// TODO: Dialogue Node with Text
// TODO: Simple Text Popup
// TODO: Cinematic Black Bars - Top Bottom - For Dialogue Mode

#include "com/_.c"
#include "ins/_.c"
#include "pre/_.c"
#include "sys/_.c"

zox_begin_module(Dialogues)
    add_hook_spawn_prefabs(spawn_prefabs_dialogues);
    define_components_dialogues(world);
    define_systems_dialogues(world);
zox_end_module(Dialogues)

#endif