#ifndef zoxm_dialogues
#define zoxm_dialogues

// TODO: NodeTree prefab as DialogueTree
// TODO: Dialogue Node with Text
// TODO: Simple Text Popup
// TODO: Cinematic Black Bars - Top Bottom - For Dialogue Mode

// NOTE: A Process Entity gets spawned for dialogue:
//      - The current leaf
//      - Any relevant links, speaker 1, speaker 2, etc

#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "tst/_.c"

zox_begin_module(Dialogues)
    define_components_dialogues(world);
    define_systems_dialogues(world);
    add_hook_spawn_prefabs(spawn_prefabs_dialogues);
zox_end_module(Dialogues)

#endif