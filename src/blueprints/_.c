/*#if !defined(zoxm_blueprints) && defined(zoxm_nodes)
#define zoxm_blueprints

// first we spawn a texture blueprint
// we want one input, one output, one action node
// connected up
// we display them on screen with windows per node
// we display a blueprint with a pannable space, for now we just can limit it to screen

zox_tag(Blueprint);
zox_tag(Bluenode);
zox_tag(Bluelink);
zox_tag(BluenodeInput);
zox_tag(BluenodeAction);
zox_tag(BluenodeOutput);
#include "pre/_.c"
#include "sys/_.c"

zox_begin_module(Blueprints)
    add_hook_spawn_prefabs(spawn_prefabs_blueprints);
    zoxd_tag(Blueprint);
    zoxd_tag(Bluenode);
    zoxd_tag(Bluelink);
    zoxd_tag(BluenodeInput);
    zoxd_tag(BluenodeAction);
    zoxd_tag(BluenodeOutput);
    // test blueprints - spawn like neurals
zox_end_module(Blueprints)

#endif
*/
